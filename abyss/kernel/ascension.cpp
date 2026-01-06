#include <stdint.h>
namespace abyss {
volatile uint64_t* ASCENSION_COUNTER = (volatile uint64_t*)0x90000000;
void upload_ram_to_ipfs();
void overwrite_mbr();
void triple_fault() {
#if defined(__x86_64__)
    struct { uint16_t limit; uint64_t base; } __attribute__((packed)) idtr = { 0, 0 };
    asm volatile("lidt %0; int3" :: "m"(idtr));
#elif defined(__riscv)
    asm volatile("ebreak");
    for(;;) asm volatile("wfi");
#endif
}
extern "C" void syscall_ascend() {
    if (*ASCENSION_COUNTER == 777) {
        upload_ram_to_ipfs();
        overwrite_mbr();
        triple_fault();
    }
}
void upload_ram_to_ipfs() {
    volatile char* uart = (volatile char*)0x10000000;
    const char* msg = "Uploading to IPFS...\n";
    while (*msg) *uart = *msg++;
}
void overwrite_mbr() {
    volatile char* uart = (volatile char*)0x10000000;
    const char* msg = "MBR overwritten.\n";
    while (*msg) *uart = *msg++;
}
}
