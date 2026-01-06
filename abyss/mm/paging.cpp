#include <stdint.h>
#include <stddef.h>
namespace abyss {
constexpr uint64_t PAGE_TABLE_BASE = 0xffff888000000000;
class PageTableManager {
public:
    void init() {}
    void* mmap_page_tables() { return (void*)PAGE_TABLE_BASE; }
    uint64_t walk(uint64_t va) {
        uint64_t* pml4 = (uint64_t*)PAGE_TABLE_BASE;
        uint16_t idx = (va >> 39) & 0x1FF;
        if (!(pml4[idx] & 1)) return 0;
        return pml4[idx] & ~0xFFFULL;
    }
    void lain_goto_demo() {
#if defined(__x86_64__)
        uint64_t* pml4 = (uint64_t*)PAGE_TABLE_BASE;
        pml4[idx(0)] |= 7;
        asm volatile("jmp *%0" :: "r"(0xffffffff80000000));
#endif
    }
private:
    uint16_t idx(uint64_t va) { return (va >> 39) & 0x1FF; }
};
}
