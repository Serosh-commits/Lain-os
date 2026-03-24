#include "mm/vmm.h"
#include "mm/pmm.h"
#include "lib/string.h"
#include "drivers/uart.h"

pagetable_t kernel_pagetable;

pte_t* vmm_walk(pagetable_t pagetable, uint64_t va, int alloc) {
    if (va >= MAXVA) return NULL;
    for (int level = 2; level > 0; level--) {
        pte_t* pte = &pagetable[PX(level, va)];
        if (*pte & PTE_V) {
            pagetable = (pagetable_t)PTE2PA(*pte);
        } else {
            if (!alloc || (pagetable = pmm_alloc()) == NULL) return NULL;
            memset(pagetable, 0, PGSIZE);
            *pte = PA2PTE(pagetable) | PTE_V;
        }
    }
    return &pagetable[PX(0, va)];
}

int vmm_map(pagetable_t pagetable, uint64_t va, uint64_t pa, uint64_t size, int perm) {
    uint64_t a = va & ~(PGSIZE - 1);
    uint64_t last = (va + size - 1) & ~(PGSIZE - 1);
    for (;;) {
        pte_t* pte = vmm_walk(pagetable, a, 1);
        if (pte == NULL) return -1;
        if (*pte & PTE_V) return -1;
        *pte = PA2PTE(pa) | perm | PTE_V | PTE_A | PTE_D;
        if (a == last) break;
        a += PGSIZE;
        pa += PGSIZE;
    }
    return 0;
}

void vmm_unmap(pagetable_t pagetable, uint64_t va, uint64_t size) {
    uint64_t a = va & ~(PGSIZE - 1);
    uint64_t last = (va + size - 1) & ~(PGSIZE - 1);
    for (;;) {
        pte_t* pte = vmm_walk(pagetable, a, 0);
        if (pte == NULL || (*pte & PTE_V) == 0) return;
        if (PTE_FLAGS(*pte) == PTE_V) return;
        uint64_t pa = PTE2PA(*pte);
        pmm_free((void*)pa);
        *pte = 0;
        if (a == last) break;
        a += PGSIZE;
    }
}

uint64_t vmm_translate(pagetable_t pagetable, uint64_t va) {
    pte_t* pte = vmm_walk(pagetable, va, 0);
    if (pte == NULL || (*pte & PTE_V) == 0) return 0;
    return PTE2PA(*pte) | (va & (PGSIZE - 1));
}

pagetable_t vmm_create() {
    pagetable_t pagetable = pmm_alloc();
    if (pagetable) {
        memset(pagetable, 0, PGSIZE);
        if (kernel_pagetable) {
            for (int i = 0; i < 512; i++) {
                if (kernel_pagetable[i] & PTE_V) {
                    pagetable[i] = kernel_pagetable[i];
                }
            }
        }
    }
    return pagetable;
}

void vmm_free(pagetable_t pagetable, int level) {
    for (int i = 0; i < 512; i++) {
        pte_t pte = pagetable[i];
        if (level == 2 && kernel_pagetable && pagetable[i] == kernel_pagetable[i]) {
            continue;
        }
        if ((pte & PTE_V) && (pte & (PTE_R | PTE_W | PTE_X)) == 0) {
            uint64_t child = PTE2PA(pte);
            vmm_free((pagetable_t)child, level - 1);
            pagetable[i] = 0;
        } else if (pte & PTE_V) {
            uint64_t pa = PTE2PA(pte);
            pmm_free((void*)pa);
            pagetable[i] = 0;
        }
    }
    pmm_free((void*)pagetable);
}

static void kvmmap(uint64_t va, uint64_t pa, uint64_t sz, int perm) {
    vmm_map(kernel_pagetable, va, pa, sz, perm);
}

#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE-1))

void vmm_init() {
    kernel_pagetable = vmm_create();
    kvmmap(UART0, UART0, PGSIZE, PTE_R | PTE_W);
    kvmmap(CLINT, CLINT, 0x10000, PTE_R | PTE_W);
    kvmmap(PLIC, PLIC, 0x400000, PTE_R | PTE_W);
    extern char etext[];
    uint64_t text_end = PGROUNDUP((uint64_t)etext);
    kvmmap(0x80000000L, 0x80000000L, text_end - 0x80000000L, PTE_R | PTE_X);
    kvmmap(text_end, text_end, 0x88000000L - text_end, PTE_R | PTE_W);
    w_satp(MAKE_SATP(kernel_pagetable));
    sfence_vma();
}
