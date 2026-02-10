#ifndef RISCV_H
#define RISCV_H

#include "lib/types.h"

#define PGSIZE 4096
#define PGSHIFT 12

#define PXMASK 0x1FF
#define PXSHIFT(level) (PGSHIFT + (9 * (level)))
#define PX(level, va) ((((uint64_t)(va)) >> PXSHIFT(level)) & PXMASK)

#define MAXVA (1L << (9 + 9 + 9 + 12 - 1))

#define PTE_V (1L << 0)
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)
#define PTE_U (1L << 4)
#define PTE_G (1L << 5)
#define PTE_A (1L << 6)
#define PTE_D (1L << 7)

#define PA2PTE(pa) ((((uint64_t)pa) >> 12) << 10)
#define PTE2PA(pte) (((pte) >> 10) << 12)
#define PTE_FLAGS(pte) ((pte) & 0x3FF)

typedef uint64_t pte_t;
typedef uint64_t *pagetable_t;

#define SATP_SV39 (8L << 60)
#define MAKE_SATP(pagetable) (SATP_SV39 | (((uint64_t)pagetable) >> 12))

static inline uint64_t r_mhartid() {
    uint64_t x;
    asm volatile("csrr %0, mhartid" : "=r" (x));
    return x;
}

static inline uint64_t r_sstatus() {
    uint64_t x;
    asm volatile("csrr %0, sstatus" : "=r" (x));
    return x;
}

static inline void w_sstatus(uint64_t x) {
    asm volatile("csrw sstatus, %0" : : "r" (x));
}

static inline uint64_t r_sip() {
    uint64_t x;
    asm volatile("csrr %0, sip" : "=r" (x));
    return x;
}

static inline void w_sip(uint64_t x) {
    asm volatile("csrw sip, %0" : : "r" (x));
}

static inline uint64_t r_sie() {
    uint64_t x;
    asm volatile("csrr %0, sie" : "=r" (x));
    return x;
}

static inline void w_sie(uint64_t x) {
    asm volatile("csrw sie, %0" : : "r" (x));
}

static inline uint64_t r_sepc() {
    uint64_t x;
    asm volatile("csrr %0, sepc" : "=r" (x));
    return x;
}

static inline void w_sepc(uint64_t x) {
    asm volatile("csrw sepc, %0" : : "r" (x));
}

static inline uint64_t r_scause() {
    uint64_t x;
    asm volatile("csrr %0, scause" : "=r" (x));
    return x;
}

static inline uint64_t r_stval() {
    uint64_t x;
    asm volatile("csrr %0, stval" : "=r" (x));
    return x;
}

static inline uint64_t r_satp() {
    uint64_t x;
    asm volatile("csrr %0, satp" : "=r" (x));
    return x;
}

static inline void w_satp(uint64_t x) {
    asm volatile("csrw satp, %0" : : "r" (x));
}

static inline void w_sscratch(uint64_t x) {
    asm volatile("csrw sscratch, %0" : : "r" (x));
}

static inline void sfence_vma() {
    asm volatile("sfence.vma zero, zero");
}

static inline uint64_t r_time() {
    uint64_t x;
    asm volatile("rdtime %0" : "=r" (x));
    return x;
}

static inline void intr_on() {
    w_sstatus(r_sstatus() | 0x2);
}

static inline void intr_off() {
    w_sstatus(r_sstatus() & ~0x2);
}

#define UART0 0x10000000L
#define CLINT 0x2000000L
#define PLIC 0x0c000000L

#endif
