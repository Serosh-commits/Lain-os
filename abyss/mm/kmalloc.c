#include "mm/kmalloc.h"
#include "mm/pmm.h"
#include "lib/string.h"
#include "kernel/riscv.h"

#define BLOCK_MAGIC 0xDEADBEEF

struct block_header {
    size_t size;
    uint32_t magic;
    int free;
    struct block_header* next;
};

static struct block_header* heap_start = NULL;

void kmalloc_init() {
    heap_start = NULL;
}

void* kmalloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    
    size_t total_size = size + sizeof(struct block_header);
    
    if (total_size <= PGSIZE - sizeof(struct block_header)) {
        struct block_header* current = heap_start;
        struct block_header* prev = NULL;
        
        while (current) {
            if (current->magic != BLOCK_MAGIC) {
                return NULL;
            }
            if (current->free && current->size >= size) {
                current->free = 0;
                return (void*)(current + 1);
            }
            prev = current;
            current = current->next;
        }
        
        void* page = pmm_alloc();
        if (!page) {
            return NULL;
        }
        
        struct block_header* block = (struct block_header*)page;
        block->size = PGSIZE - sizeof(struct block_header);
        block->magic = BLOCK_MAGIC;
        block->free = 0;
        block->next = NULL;
        
        if (prev) {
            prev->next = block;
        } else {
            heap_start = block;
        }
        
        return (void*)(block + 1);
    } else {
        size_t pages = (total_size + PGSIZE - 1) / PGSIZE;
        void* ptr = pmm_alloc();
        if (!ptr) {
            return NULL;
        }
        
        for (size_t i = 1; i < pages; i++) {
            void* p = pmm_alloc();
            if (!p) {
                return NULL;
            }
        }
        
        return ptr;
    }
}

void kfree(void* ptr) {
    if (!ptr) {
        return;
    }
    
    struct block_header* block = (struct block_header*)ptr - 1;
    
    if (block->magic == BLOCK_MAGIC) {
        block->free = 1;
    }
}
