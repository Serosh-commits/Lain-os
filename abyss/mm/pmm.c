#include "mm/pmm.h"
#include "kernel/riscv.h"
#include "lib/string.h"

struct run {
    struct run* next;
};

static struct run* freelist;
static uint64_t free_pages;

extern char end[];

void pmm_init(uint64_t start_addr, uint64_t end_addr) {
    freelist = NULL;
    free_pages = 0;
    
    uint64_t p = start_addr;
    p = (p + PGSIZE - 1) & ~(PGSIZE - 1);
    uint64_t end_p = end_addr & ~(PGSIZE - 1);
    
    for (uint64_t i = end_p; i > p; ) {
        i -= PGSIZE;
        pmm_free((void*)i);
    }
}

void* pmm_alloc() {
    struct run* r = freelist;
    
    if (r) {
        freelist = r->next;
        free_pages--;
        memset((char*)r, 0, PGSIZE);
    }
    
    return (void*)r;
}

void pmm_free(void* pa) {
    uint64_t addr = (uint64_t)pa;
    
    if (addr % PGSIZE != 0 || addr < (uint64_t)end || addr >= 0x88000000) {
        return;
    }
    
    memset((char*)pa, 1, PGSIZE);
    
    struct run* r = (struct run*)pa;
    r->next = freelist;
    freelist = r;
    free_pages++;
}

uint64_t pmm_free_count() {
    return free_pages;
}
