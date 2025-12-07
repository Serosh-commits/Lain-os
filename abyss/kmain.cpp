#include "kernel/scheduler.cpp"
#include "fs/neurovfs.cpp"
#include "kernel/ego.cpp"
#include "drivers/quantum.cpp"
#include "kernel/shell.cpp"
#include "mm/paging.cpp"
#include "kernel/ascension.cpp"
#include "kernel/patcher.cpp"
#include "kernel/panic.cpp"
#include "mm/heap.cpp"

abyss::TemporalScheduler scheduler;
abyss::NeuroVFS vfs;
abyss::EgoManager ego_manager;
abyss::QuantumDriver quantum_driver;
abyss::PredictiveShell shell;
abyss::PageTableManager page_table_manager;
abyss::Decompiler decompiler;
abyss::HotPatcher hot_patcher;

extern "C" void kmain() {
    volatile char* uart = (volatile char*)0x10000000;
    const char* msg = "LainOS/Abyss: Connection to the Wired established.\n";
    while (*msg) *uart = *msg++;

    scheduler.init();
    ego_manager.init();
    quantum_driver.init();
    shell.init();
    page_table_manager.init();

    // Main loop
    for (;;) {
        scheduler.schedule();
        asm volatile("wfi");
    }
}
