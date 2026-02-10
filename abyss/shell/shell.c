#include "kernel/shell.h"
#include "kernel/process.h"
#include "drivers/uart.h"
#include "fs/vfs.h"
#include "lib/string.h"
#include "mm/pmm.h"

#define CHAIN_LEN 3

extern void markov_init();
extern void markov_learn(const char* text);
extern char markov_predict(const char* text);

static char cmdline[256];
static char history[16][256];
static int history_idx = 0;

static void print_banner() {
    uart_puts("\n");
    uart_puts("        _          _        ___  ____  \n");
    uart_puts("       | |    __ _(_)_ __  / _ \\/ ___| \n");
    uart_puts("       | |   / _` | | '_ \\| | | \\___ \\ \n");
    uart_puts("       | |__| (_| | | | | | |_| |___) |\n");
    uart_puts("       |_____\\__,_|_|_| |_|\\___/|____/ \n");
    uart_puts("\n");
    uart_puts("       \"No matter where you are,\n");
    uart_puts("        everyone is always connected.\"\n");
    uart_puts("\n");
    uart_puts("       Present Day. Present Time.\n");
    uart_puts("       HA-HA-HA-HA-HA.\n");
    uart_puts("\n");
}

static void cmd_help() {
    uart_puts("Available commands:\n");
    uart_puts("  help       - Show this help\n");
    uart_puts("  clear      - Clear screen\n");
    uart_puts("  neofetch   - System information\n");
    uart_puts("  ps         - List processes\n");
    uart_puts("  kill       - Kill process\n");
    uart_puts("  mem        - Memory status\n");
    uart_puts("  timeline   - Timeline management\n");
    uart_puts("  echo       - Echo text\n");
    uart_puts("  ascend     - Trigger ascension\n");
}

static void cmd_clear() {
    uart_puts("\033[2J\033[H");
}

static void cmd_neofetch() {
    uart_puts("\n");
    uart_puts("    .--.      root@abyss\n");
    uart_puts("   |o_o |     -------------\n");
    uart_puts("   |:_/ |     OS: Lain-OS (Abyss)\n");
    uart_puts("  //   \\ \\    Kernel: RISC-V 64\n");
    uart_puts(" (|     | )   Uptime: Unknown\n");
    uart_puts("/'\\_ _/`\\    Memory: ");
    uart_putnum(pmm_free_count() * 4, 10);
    uart_puts(" KB free\n");
    uart_puts("\\___)=(___/   Shell: Predictive Shell v1.0\n");
    uart_puts("\n");
}

static void cmd_ps() {
    uart_puts("PID  STATE      NAME\n");
    for (int i = 0; i < NPROC; i++) {
        struct proc* p = &proc_table[i];
        if (p->state != UNUSED) {
            uart_putnum(p->pid, 10);
            uart_puts("  ");
            switch (p->state) {
                case SLEEPING:
                    uart_puts("SLEEPING  ");
                    break;
                case RUNNABLE:
                    uart_puts("RUNNABLE  ");
                    break;
                case RUNNING:
                    uart_puts("RUNNING   ");
                    break;
                case ZOMBIE:
                    uart_puts("ZOMBIE    ");
                    break;
                default:
                    uart_puts("UNKNOWN   ");
            }
            uart_puts(p->name);
            uart_puts("\n");
        }
    }
}

static void cmd_kill(int pid) {
    if (proc_kill(pid) == 0) {
        uart_puts("Process killed\n");
    } else {
        uart_puts("Process not found\n");
    }
}

static void cmd_mem() {
    uart_puts("Memory Status:\n");
    uart_puts("  Free pages: ");
    uart_putnum(pmm_free_count(), 10);
    uart_puts("\n");
    uart_puts("  Free memory: ");
    uart_putnum(pmm_free_count() * 4, 10);
    uart_puts(" KB\n");
}

static void cmd_ascend() {
    uart_puts("\n");
    uart_puts("Initiating ascension sequence...\n");
    uart_puts("Dumping system state...\n");
    uart_puts("\n");
    cmd_ps();
    uart_puts("\n");
    cmd_mem();
    uart_puts("\n");
    uart_puts("Ascension complete. Returning to the Wired...\n");
    uart_puts("\n");
}

static void cmd_echo(char* args) {
    uart_puts(args);
    uart_puts("\n");
}

static int atoi(const char* s) {
    int n = 0;
    while (*s >= '0' && *s <= '9') {
        n = n * 10 + (*s - '0');
        s++;
    }
    return n;
}

void shell_execute(char* cmd) {
    char* argv[16];
    int argc = 0;
    
    char* p = cmd;
    while (*p && argc < 16) {
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (!*p) {
            break;
        }
        
        argv[argc++] = p;
        
        while (*p && *p != ' ' && *p != '\t') {
            p++;
        }
        if (*p) {
            *p++ = 0;
        }
    }
    
    if (argc == 0) {
        return;
    }
    
    if (strcmp(argv[0], "help") == 0) {
        cmd_help();
    } else if (strcmp(argv[0], "clear") == 0) {
        cmd_clear();
    } else if (strcmp(argv[0], "neofetch") == 0) {
        cmd_neofetch();
    } else if (strcmp(argv[0], "ps") == 0) {
        cmd_ps();
    } else if (strcmp(argv[0], "kill") == 0) {
        if (argc > 1) {
            cmd_kill(atoi(argv[1]));
        } else {
            uart_puts("Usage: kill <pid>\n");
        }
    } else if (strcmp(argv[0], "mem") == 0) {
        cmd_mem();
    } else if (strcmp(argv[0], "ascend") == 0) {
        cmd_ascend();
    } else if (strcmp(argv[0], "echo") == 0) {
        if (argc > 1) {
            cmd_echo(argv[1]);
        }
    } else if (strcmp(argv[0], "timeline") == 0) {
        uart_puts("Timeline feature not yet implemented\n");
    } else {
        uart_puts("Unknown command: ");
        uart_puts(argv[0]);
        uart_puts("\n");
        uart_puts("Type 'help' for available commands\n");
    }
}

void shell_init() {
    markov_init();
    history_idx = 0;
}

void shell_run() {
    print_banner();
    
    while (1) {
        uart_puts("root@abyss# ");
        
        int pos = 0;
        memset(cmdline, 0, sizeof(cmdline));
        
        while (1) {
            int c = uart_getc();
            if (c == -1) {
                continue;
            }
            
            if (c == '\n' || c == '\r') {
                uart_puts("\n");
                break;
            } else if (c == '\t') {
                if (pos >= CHAIN_LEN) {
                    char pred = markov_predict(cmdline);
                    if (pred && pred != '\n' && pred != '\r') {
                        cmdline[pos++] = pred;
                        uart_putc(pred);
                    }
                }
            } else if (c == 127 || c == '\b') {
                if (pos > 0) {
                    pos--;
                    cmdline[pos] = 0;
                    uart_puts("\b \b");
                }
            } else if (c >= 32 && c < 127 && pos < 255) {
                cmdline[pos++] = c;
                uart_putc(c);
            }
        }
        
        cmdline[pos] = 0;
        
        if (pos > 0) {
            strcpy(history[history_idx % 16], cmdline);
            history_idx++;
            
            markov_learn(cmdline);
            
            shell_execute(cmdline);
        }
    }
}
