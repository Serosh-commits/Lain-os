#ifndef KMALLOC_H
#define KMALLOC_H

#include "lib/types.h"

void kmalloc_init();
void* kmalloc(size_t size);
void kfree(void* ptr);

#endif
