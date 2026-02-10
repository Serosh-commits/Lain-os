#include "kernel/riscv.h"
#include "kernel/process.h"
#include "kernel/scheduler.h"
#include "kernel/shell.h"
#include "drivers/uart.h"
#include "mm/pmm.h"
#include "mm/vmm.h"
#include "mm/kmalloc.h"
#include "fs/vfs.h"

extern char end[];
extern void timer_init();

void kmain() {
    uart_init();
    
    uart_puts("\n");
    uart_puts("[BOOT] Lain-OS Kernel Starting...\n");
    
    uart_puts("[BOOT] Initializing physical memory...\n");
    pmm_init((uint64_t)end, 0x88000000L);
    
    uart_puts("[BOOT] Initializing virtual memory...\n");
    vmm_init();
    
    uart_puts("[BOOT] Initializing kernel allocator...\n");
    kmalloc_init();
    
    uart_puts("[BOOT] Initializing VFS...\n");
    vfs_init();
    
    uart_puts("[BOOT] Initializing process manager...\n");
    proc_init();
    
    uart_puts("[BOOT] Initializing scheduler...\n");
    sched_init();
    
    uart_puts("[BOOT] Initializing timer...\n");
    timer_init();
    
    uart_puts("[BOOT] Enabling interrupts...\n");
    intr_on();
    
    uart_puts("[BOOT] Boot sequence complete!\n");
    uart_puts("\n");
    
    shell_init();
    shell_run();
    
    while (1) {
        asm volatile("wfi");
    }
}
