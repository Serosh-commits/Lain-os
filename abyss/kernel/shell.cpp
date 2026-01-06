#include "../kernel.hpp"
#include <stdint.h>

namespace abyss {

static uint8_t markov_chain[256][256][256]; 
class PredictiveShell {
    char history[3];
    char buffer[128];
    int buf_idx = 0;

    void process_command() {
        volatile char* uart = (volatile char*)0x10000000;
        if (buf_idx > 0) {
            if (buffer[0] == 'p' && buffer[1] == 'a' && buffer[2] == 'n' && buffer[3] == 'i' && buffer[4] == 'c') {
                panic("User requested panic");
            } else if (buffer[0] == 'a' && buffer[1] == 's' && buffer[2] == 'c' && buffer[3] == 'e' && buffer[4] == 'n' && buffer[5] == 'd') {
                syscall_ascend();
            } else if (buffer[0] == 's' && buffer[1] == 'p' && buffer[2] == 'a' && buffer[3] == 'w' && buffer[4] == 'n') {
                scheduler.spawn(proc_count);
                const char* m = "Process spawned.\n";
                while (*m) *uart = *m++;
            } else if (buffer[0] == 'q' && buffer[1] == 'u' && buffer[2] == 'e' && buffer[3] == 'r' && buffer[4] == 'y') {
                const char* m = quantum_driver.is_entangled() ? "Entangled.\n" : "Stable.\n";
                while (*m) *uart = *m++;
            } else if (buffer[0] == 'u' && buffer[1] == 'p' && buffer[2] == 't' && buffer[3] == 'i' && buffer[4] == 'm' && buffer[5] == 'e') {
                char tbuf[32];
                itoa(tbuf, syscall_uptime());
                const char* m = "Uptime: ";
                while (*m) *uart = *m++;
                char* tb = tbuf;
                while (*tb) *uart = *tb++;
                *uart = '\n';
            } else {
                const char* unk = "Unknown command.\n";
                while (*unk) *uart = *unk++;
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
