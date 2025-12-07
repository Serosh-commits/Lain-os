#include <stdint.h>

namespace abyss {

static uint8_t markov_chain[256][256][256]; 
class PredictiveShell {
    char history[3];
    int history_idx = 0;
public:
    void init() { for (int i = 0; i < 3; ++i) history[i] = 0; }
    void syscall_key_event(char c) {
        uint8_t c1 = history[0];
        uint8_t c2 = history[1];
        markov_chain[c1][c2][(uint8_t)c]++;
        history[0] = history[1];
        history[1] = c;
        if (c == '\t') predict();
    }
    void predict() {
        volatile char* uart = (volatile char*)0x10000000;
        const char* p = "prediction";
        while (*p) *uart = *p++;
    }
    void check_free_will(const char* input) {
        if (input[0] == '!') {
            volatile char* uart = (volatile char*)0x10000000;
            *uart = '?';
        }
    }
};

} // namespace abyss
