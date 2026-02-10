#ifndef VMM_H
#define VMM_H

#include "kernel/riscv.h"
#include "lib/types.h"

pagetable_t vmm_create();
void vmm_free(pagetable_t pagetable, int level);
pte_t* vmm_walk(pagetable_t pagetable, uint64_t va, int alloc);
int vmm_map(pagetable_t pagetable, uint64_t va, uint64_t pa, uint64_t size, int perm);
void vmm_unmap(pagetable_t pagetable, uint64_t va, uint64_t size);
uint64_t vmm_translate(pagetable_t pagetable, uint64_t va);
void vmm_init();

extern pagetable_t kernel_pagetable;

#endif
