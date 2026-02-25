#include "kernel/riscv.h"
#include "kernel/process.h"
#include "kernel/scheduler.h"
#include "kernel/shell.h"
#include "drivers/uart.h"
#include "mm/pmm.h"
#include "mm/vmm.h"
#include "mm/kmalloc.h"
#include "fs/vfs.h"
#include "lib/string.h"

extern char end[];
extern void timer_init();
extern void scheduler();

void shell_task() {
    shell_run();
}

void kmain() {
    uart_init();
    uart_puts("\n");
    uart_puts("[BOOT] Lain-OS Kernel Starting...\n");
    pmm_init((uint64_t)end, 0x88000000L);
    vmm_init();
    kmalloc_init();
    vfs_init();
    proc_init();
    sched_init();
    timer_init();
    intr_on();
    uart_puts("[BOOT] Boot sequence complete!\n");
    uart_puts("\n");
    
    shell_init();
    struct proc* p = proc_alloc();
    if (p) {
        strcpy(p->name, "shell");
        p->context.ra = (uint64_t)shell_task;
    }
    
    scheduler();
}
