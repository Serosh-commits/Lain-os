#include "kernel.hpp"
#include "stl/runtime.cpp"
#include "kernel/panic.cpp"
#include "mm/heap.cpp"
#include "kernel/ascension.cpp"
#include "kernel/scheduler.cpp"
#include "fs/neurovfs.cpp"
#include "kernel/ego.cpp"
#include "drivers/quantum.cpp"
#include "kernel/shell.cpp"
#include "mm/paging.cpp"
#include "kernel/patcher.cpp"

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
    using namespace abyss;
    volatile char* uart = (volatile char*)0x10000000;
    const char* msg = "LainOS/Abyss: Connection to the Wired established.\n";
    while (*msg) *uart = *msg++;

    scheduler.init();
    ego_manager.init();
    quantum_driver.init();
    shell.init();
    page_table_manager.init();

    const char* prompt = "\nroot@abyss# ";
    const char* p = prompt;
    while (*p) *uart = *p++;

    for (;;) {
        scheduler.schedule();
        volatile char* uart_lsr = (volatile char*)0x10000005;
        if (*uart_lsr & 1) {
            char c = *(volatile char*)0x10000000;
            shell.syscall_key_event(c);
        }
    }
}
