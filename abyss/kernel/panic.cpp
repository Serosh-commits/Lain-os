#include <stdint.h>
namespace abyss {
struct DWARFContext { uint64_t debug_info_addr; uint64_t debug_abbrev_addr; };
void unwind_stack() {}
extern "C" void panic(const char* message) {
    (void)message;
#if defined(__x86_64__)
    asm volatile("cli" ::: "memory");
    unwind_stack();
    for (;;) asm volatile("hlt");
#elif defined(__riscv)
    asm volatile("csrc sstatus, 0x2" ::: "memory");
    unwind_stack();
    for (;;) asm volatile("wfi");
#else
    for (;;) {}
#endif
}
}
