#include <stdint.h>
#include <stddef.h>

namespace abyss {

constexpr uint64_t PAGE_TABLE_BASE = 0xffff888000000000;
class PageTableManager {
public:
    void init() {}
    void* mmap_page_tables() { return (void*)PAGE_TABLE_BASE; }
    void lain_goto_demo() {
        uint64_t* pml4 = (uint64_t*)PAGE_TABLE_BASE;
        pml4[511] |= 4;
        asm volatile("jmp *%0" :: "r"(0xffffffff80000000));
    }
};

} // namespace abyss
