#include "../kernel.hpp"
#include <stdint.h>

namespace abyss {

static uint8_t markov_chain[256][256][256]; 
class PredictiveShell {
    char history[3];
    char buffer[128];
    int buf_idx = 0;

    int strncmp(const char* s1, const char* s2, int n) {
        for (int i=0; i<n; i++) {
            if (s1[i] != s2[i]) return s1[i] - s2[i];
            if (s1[i] == 0) return 0;
        }
        return 0;
    }
    
    uint64_t atoh(const char* s) {
        uint64_t res = 0;
        if (s[0] == '0' && s[1] == 'x') s += 2;
        while (*s) {
            res *= 16;
            if (*s >= '0' && *s <= '9') res += (*s - '0');
            else if (*s >= 'a' && *s <= 'f') res += (*s - 'a' + 10);
            else if (*s >= 'A' && *s <= 'F') res += (*s - 'A' + 10);
            s++;
        }
        return res;
    }

    uint64_t atoi(const char* s) {
        uint64_t res = 0;
        while (*s) {
            if (*s < '0' || *s > '9') break;
            res = res * 10 + (*s - '0');
            s++;
        }
        return res;
    }

    void process_command() {
        volatile char* uart = (volatile char*)0x10000000;
        if (buf_idx > 0) {
            buffer[buf_idx] = 0;
            
            // Split args
            char* args[8];
            int argc = 0;
            char* cur = buffer;
            while (*cur && argc < 8) {
                while (*cur == ' ') cur++;
                if (!*cur) break;
                args[argc++] = cur;
                while (*cur && *cur != ' ') cur++;
                if (*cur) *cur++ = 0;
            }

            if (argc > 0) {
                const char* cmd = args[0];

                if (strncmp(cmd, "panic", 5) == 0) {
                    panic("User requested panic");
                } else if (strncmp(cmd, "ascend", 6) == 0) {
                    syscall_ascend();
                } else if (strncmp(cmd, "spawn", 5) == 0) {
                    scheduler.spawn(proc_count);
                    const char* m = "Process spawned.\n"; while(*m) *uart = *m++;
                } else if (strncmp(cmd, "query", 5) == 0) {
                    const char* m = quantum_driver.is_entangled() ? "Entangled.\n" : "Stable.\n"; while(*m) *uart = *m++;
                } else if (strncmp(cmd, "uptime", 6) == 0) {
                    char tbuf[32]; itoa(tbuf, syscall_uptime());
                    const char* m = "Uptime: "; while(*m) *uart = *m++;
                    char* tb = tbuf; while(*tb) *uart = *tb++; *uart = '\n';
                } else if (strncmp(cmd, "help", 4) == 0) {
                    const char* h = "Commands: panic, ascend, spawn, query, uptime, help, info\n"
                                    "Files: ls, mkdir, cd, pwd, touch, write, cat, rm, cp, mv, > redirection\n"
                                    "System: ps, kill, clear, mem, hostname, neofetch, yay, sleep\n"
                                    "Debug: decompile <addr>, patch <addr> <val>\n";
                    while(*h) *uart = *h++;
                } else if (strncmp(cmd, "info", 4) == 0) {
                    const char* i = "LainOS/Abyss v1.0.0-wired\nBuilt with C++20/RISC-V\nCopyright (C) 1998, 2025 Wired Foundation\n";
                    while(*i) *uart = *i++;
                } else if (strncmp(cmd, "ls", 2) == 0) {
                    vfs.list();
                } else if (strncmp(cmd, "cp", 2) == 0) {
                    if (argc > 2) {
                        if (!vfs.copy_file(args[1], args[2])) {
                            const char* m = "Copy failed.\n"; while(*m) *uart = *m++;
                        }
                    }
                } else if (strncmp(cmd, "mv", 2) == 0) {
                    if (argc > 2) {
                        if (!vfs.rename_file(args[1], args[2])) {
                            const char* m = "Move failed.\n"; while(*m) *uart = *m++;
                        }
                    }
                } else if (strncmp(cmd, "sleep", 5) == 0) {
                    if (argc > 1) {
                        uint64_t ms = atoi(args[1]) * 1000000;
                        volatile uint64_t i = 0;
                        while (i < ms) i = i + 1;
                    }
                } else if (strncmp(cmd, "hostname", 8) == 0) {
                    const char* m = "abyss\n"; while(*m) *uart = *m++;
                } else if (strncmp(cmd, "neofetch", 8) == 0) {
                    const char* n = 
                        "       .---.       root@abyss\n"
                        "      /     \\      ----------\n"
                        "      | (O) |      OS: LainOS/Abyss RISC-V\n"
                        "      \\  ^  /      Kernel: 1.0.0-wired\n"
                        "       '---'       Shell: Predictive\n"
                        "      /| L |\\      Uptime: wired-inf\n"
                        "     / | A | \\     Memory: 512MB\n"
                        "    /  | I |  \\\n"
                        "       | N |\n";
                    while(*n) *uart = *n++;
                } else if (strncmp(cmd, "mkdir", 5) == 0) {
                     if (argc > 1) {
                         if (vfs.mkdir(args[1])) {
                             const char* m = "Directory created.\n"; while(*m) *uart = *m++;
                         } else {
                             const char* m = "Failed (exists?).\n"; while(*m) *uart = *m++;
                         }
                     }
                } else if (strncmp(cmd, "cd", 2) == 0) {
                     if (argc > 1) {
                         if (!vfs.change_dir(args[1])) {
                             const char* m = "Directory not found.\n"; while(*m) *uart = *m++;
                         }
                     } else {
                         vfs.change_dir("/");
                     }
                } else if (strncmp(cmd, "pwd", 3) == 0) {
                    vfs.pwd();
                } else if (strncmp(cmd, "touch", 5) == 0) {
                    if (argc > 1) {
                         if (vfs.create_file(args[1])) {
                             const char* m = "File created.\n"; while(*m) *uart = *m++;
                         } else {
                             const char* m = "Failed (exists?).\n"; while(*m) *uart = *m++;
                         }
                    } else {
                        const char* m = "Usage: touch <name>\n"; while(*m) *uart = *m++;
                    }
                } else if (strncmp(cmd, "echo", 4) == 0) {
                    int redir_idx = -1;
                    for (int i = 0; i < argc; i++) {
                        if (strncmp(args[i], ">", 1) == 0) {
                            redir_idx = i;
                            break;
                        }
                    }
                    if (redir_idx > 0 && argc > redir_idx + 1) {
                        char out[128]; int out_ptr = 0;
                        for (int i = 1; i < redir_idx; i++) {
                            char* s = args[i]; 
                            while (*s && out_ptr < 127) out[out_ptr++] = *s++;
                            if (out_ptr < 127) out[out_ptr++] = ' ';
                        }
                        vfs.create_file(args[redir_idx + 1]);
                        vfs.write(args[redir_idx + 1], out, out_ptr);
                    } else {
                        for (int i = 1; i < argc; i++) {
                            char* s = args[i]; while (*s) *uart = *s++;
                            *uart = ' ';
                        }
                        *uart = '\n';
                    }
                } else if (strncmp(cmd, "whoami", 6) == 0) {
                    const char* m = "root\n"; while(*m) *uart = *m++;
                } else if (strncmp(cmd, "sudo", 4) == 0) {
                    const char* m = "You are already god.\n"; while(*m) *uart = *m++;
                } else if (strncmp(cmd, "cat", 3) == 0) {
                    if (argc > 1) {
                        char buf[128];
                        int n = vfs.read(args[1], buf, 127);
                        if (n >= 0) {
                            buf[n] = 0;
                            char* p = buf; while(*p) *uart = *p++; *uart = '\n';
                        } else {
                            const char* m = "File not found.\n"; while(*m) *uart = *m++;
                        }
                    } else {
                         const char* m = "Usage: cat <name>\n"; while(*m) *uart = *m++;
                    }
                } else if (strncmp(cmd, "write", 5) == 0) {
                    if (argc > 2) {
                        if (vfs.write(args[1], args[2], 0)) { 
                             const char* s = args[2];
                             int len = 0; while(s[len]) len++;
                             vfs.write(args[1], args[2], len);
                             const char* m = "Written.\n"; while(*m) *uart = *m++;
                        } else {
                             const char* m = "Failed.\n"; while(*m) *uart = *m++;
                        }
                    } else {
                        const char* m = "Usage: write <name> <data>\n"; while(*m) *uart = *m++;
                    }
                } else if (strncmp(cmd, "yay", 3) == 0) {
                     if (argc > 1) {
                         const char* pkg = args[argc-1]; 
                         if (argc == 1) {
                             const char* m = "Usage: yay -S <package>\n"; while(*m) *uart = *m++;
                         } else {
                             const char* m = "Resolving dependencies...\n"; while(*m) *uart = *m++;
                             const char* d = "Downloading "; while(*d) *uart = *d++; 
                             const char* p = pkg; while(*p) *uart = *p++; 
                             const char* e = " from aur.archlinux.org...\n"; while(*e) *uart = *e++;
                             
                             if (vfs.create_file(pkg)) {
                                 const char* c = "Compiling source...\nInstalling...\n"; while(*c) *uart = *c++;
                                 vfs.write(pkg, "ELF...JustKidding", 17);
                                 const char* done = "Finished.\n"; while(*done) *uart = *done++;
                             } else {
                                 const char* f = "Package already installed.\n"; while(*f) *uart = *f++;
                             }
                         }
                     }
                } else if (strncmp(cmd, "rm", 2) == 0) {
                    if (argc > 1) {
                        if (vfs.remove(args[1])) {
                             const char* m = "Deleted.\n"; while(*m) *uart = *m++;
                        } else {
                             const char* m = "Not found.\n"; while(*m) *uart = *m++;
                        }
                    }
                } else if (strncmp(cmd, "ps", 2) == 0) {
                    scheduler.print_process_list();
                } else if (strncmp(cmd, "kill", 4) == 0) {
                     if (argc > 1) {
                         uint64_t pid = atoi(args[1]);
                         if (scheduler.kill(pid)) {
                              const char* m = "Killed.\n"; while(*m) *uart = *m++;
                         } else {
                              const char* m = "Failed (Invalid PID or Perm).\n"; while(*m) *uart = *m++;
                         }
                     }
                } else if (strncmp(cmd, "mem", 3) == 0) {
                     size_t used = get_heap_usage();
                     const char* m = "Heap Used: "; while(*m) *uart = *m++;
                     char buf[32]; itoa(buf, used);
                     char* b = buf; while(*b) *uart = *b++;
                     const char* u = " bytes\n"; while(*u) *uart = *u++;
                } else if (strncmp(cmd, "clear", 5) == 0) {
                    const char* c = "\033[2J\033[H";
                    while(*c) *uart = *c++;
                } else if (strncmp(cmd, "decompile", 9) == 0) {
                    if (argc > 1) {
                        uint64_t addr = atoh(args[1]);
                        decompiler.syscall_decompile(addr);
                    } else {
                         const char* m = "Usage: decompile <hex_addr>\nExample: decompile 0x80200000\n"; while(*m) *uart = *m++;
                    }
                } else if (strncmp(cmd, "patch", 5) == 0) {
                     if (argc > 2) {
                          uint64_t addr = atoh(args[1]);
                          uint32_t val = (uint32_t)atoh(args[2]);
                          hot_patcher.syscall_hotpatch(addr, val);
                     }
                } else {
                    const char* unk = "Unknown command.\n";
                    while (*unk) *uart = *unk++;
                }
            }
        }
        buf_idx = 0;
        *uart = '\n';
        const char* prompt = "root@abyss# ";
        while (*prompt) *uart = *prompt++;
    }

    void predict() {
        uint8_t c1 = history[0], c2 = history[1];
        uint8_t best_char = 0;
        uint64_t max_freq = 0;
        for (int i = 0; i < 256; ++i) {
            if (markov_chain[c1][c2][i] > max_freq) {
                max_freq = markov_chain[c1][c2][i];
                best_char = i;
            }
        }
        volatile char* uart = (volatile char*)0x10000000;
        if (best_char) *uart = (char)best_char;
    }

public:
    void init() { 
        for (int i = 0; i < 3; ++i) history[i] = 0;
        buf_idx = 0;
    }

    void syscall_key_event(char c) {
        volatile char* uart = (volatile char*)0x10000000;
        
        if (c == 127 || c == 8) { 
            if (buf_idx > 0) {
                buf_idx--;
                *uart = '\b'; *uart = ' '; *uart = '\b';
            }
            return;
        }

        *uart = c;
        if (c == '\r' || c == '\n') {
            process_command();
            return;
        }
        if (buf_idx < 127) {
            buffer[buf_idx++] = c;
        }
        uint8_t c1 = history[0], c2 = history[1];
        markov_chain[c1][c2][(uint8_t)c]++;
        history[0] = history[1];
        history[1] = c;
        if (c == '\t') predict();
    }
};

}
