#include <stdint.h>

namespace abyss {

struct DWARFContext { uint64_t debug_info_addr; uint64_t debug_abbrev_addr; };
void unwind_stack() {}
extern "C" void panic(const char* message) {
    asm volatile("cli" ::: "memory");
    unwind_stack();
    for (;;) asm volatile("hlt");
}

} // namespace abyss
