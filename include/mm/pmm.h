#ifndef PMM_H
#define PMM_H

#include "lib/types.h"

void pmm_init(uint64_t start, uint64_t end);
void* pmm_alloc();
void pmm_free(void* pa);
uint64_t pmm_free_count();

#endif
