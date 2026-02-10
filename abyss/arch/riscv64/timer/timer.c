#include "kernel/riscv.h"
#include "lib/types.h"

#define TIMER_INTERVAL 100000

static inline void write_clint(uint64_t offset, uint64_t value) {
    volatile uint64_t* addr = (volatile uint64_t*)(CLINT + offset);
    *addr = value;
}

static inline uint64_t read_clint(uint64_t offset) {
    volatile uint64_t* addr = (volatile uint64_t*)(CLINT + offset);
    return *addr;
}

void timer_init() {
    uint64_t time = r_time();
    write_clint(0x4000, time + TIMER_INTERVAL);
    
    uint64_t sie = r_sie();
    w_sie(sie | (1 << 5));
}

void timer_handler() {
    uint64_t time = r_time();
    write_clint(0x4000, time + TIMER_INTERVAL);
}
