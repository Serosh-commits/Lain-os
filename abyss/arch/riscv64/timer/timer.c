#include "kernel/riscv.h"
#include "lib/types.h"

#define TIMER_INTERVAL 100000

static inline void sbi_set_timer(uint64_t stime_value) {
    asm volatile(
        "mv a0, %0\n"
        "li a7, 0\n"
        "ecall"
        :
        : "r"(stime_value)
        : "a0", "a7"
    );
}

void timer_init() {
    uint64_t time = r_time();
    sbi_set_timer(time + TIMER_INTERVAL);
    
    uint64_t sie = r_sie();
    w_sie(sie | (1 << 5));
}

void timer_handler() {
    uint64_t time = r_time();
    sbi_set_timer(time + TIMER_INTERVAL);
}

uint64_t get_uptime() {
    return r_time();
}
