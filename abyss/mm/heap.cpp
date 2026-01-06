#include <stdint.h>
#include <stddef.h>

namespace abyss {

constexpr size_t HEAP_SIZE = 128 * 1024 * 1024;
static uint8_t heap_memory[HEAP_SIZE];
static size_t heap_offset = 0;
void* kmalloc(size_t size) {
    size = (size + 15) & ~15;
    if (heap_offset + size > HEAP_SIZE) return nullptr;
    void* ptr = &heap_memory[heap_offset];
    heap_offset += size;
    return ptr;
}
void kfree(void* ptr) { (void)ptr; }
}
void* operator new(size_t size) { return abyss::kmalloc(size); }
void* operator new[](size_t size) { return abyss::kmalloc(size); }
void operator delete(void* ptr) noexcept { abyss::kfree(ptr); }
void operator delete(void* ptr, size_t) noexcept { abyss::kfree(ptr); }
void operator delete[](void* ptr) noexcept { abyss::kfree(ptr); }
void operator delete[](void* ptr, size_t) noexcept { abyss::kfree(ptr); }
