#include "kernel.hpp"
#include "stl/runtime.cpp"
#include "kernel/panic.cpp"
#include "mm/heap.cpp"
#include "kernel/ascension.cpp"
#include "kernel/scheduler.cpp"
#include "fs/neurovfs.cpp"
#include "kernel/ego.cpp"
#include "drivers/quantum.cpp"
#include "kernel/patcher.cpp"
#include "kernel/shell.cpp"
#include "mm/paging.cpp"

namespace abyss {
TemporalScheduler scheduler;
NeuroVFS vfs;
EgoManager ego_manager;
QuantumDriver quantum_driver;
PredictiveShell shell;
PageTableManager page_table_manager;
Decompiler decompiler;
HotPatcher hot_patcher;
}

extern "C" uint64_t syscall_uptime() {
    return abyss::ticks;
}

extern "C" void kmain() {
    volatile char* uart = (volatile char*)0x10000000;
    const char* msg = "LainOS/Abyss: Connection to the Wired established.\n";
    while (*msg) *uart = *msg++;

    abyss::scheduler.init();
    abyss::ego_manager.init();
    abyss::quantum_driver.init();
    abyss::vfs.init(); // Init files
    abyss::shell.init();
    abyss::page_table_manager.init();

    const char* prompt = "\nroot@abyss# ";
    const char* p = prompt;
    while (*p) *uart = *p++;

    for (;;) {
        abyss::scheduler.schedule();
        volatile char* uart_lsr = (volatile char*)0x10000005;
        if (*uart_lsr & 1) {
            char c = *(volatile char*)0x10000000;
            abyss::shell.syscall_key_event(c);
        }
    }
}
