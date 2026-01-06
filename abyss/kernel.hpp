#pragma once
#include <stdint.h>
#include <stddef.h>

namespace abyss {
    class TemporalScheduler;
    class NeuroVFS;
    class EgoManager;
    class QuantumDriver;
    class PredictiveShell;
    class PageTableManager;
    class Decompiler;
    class HotPatcher;

    extern TemporalScheduler scheduler;
    extern NeuroVFS vfs;
    extern EgoManager ego_manager;
    extern QuantumDriver quantum_driver;
    extern PredictiveShell shell;
    extern PageTableManager page_table_manager;
    extern Decompiler decompiler;
    extern HotPatcher hot_patcher;

    extern int proc_count;
    void itoa(char* buf, uint64_t n);

    extern "C" void panic(const char* message);
    extern "C" void syscall_ascend();
    extern "C" uint64_t syscall_uptime();
}
