#include "kernel/riscv.h"
#include "kernel/process.h"
#include "kernel/scheduler.h"
#include "kernel/syscall.h"
#include "drivers/uart.h"

extern void timer_handler();

void trap_handler(struct trapframe* tf) {
    uint64_t scause = r_scause();
    uint64_t sepc = r_sepc();
    
    if (scause & (1ULL << 63)) {
        uint64_t interrupt = scause & 0xFF;
        
        switch (interrupt) {
            case 1:
                w_sip(r_sip() & ~2);
                break;
                
            case 5:
                timer_handler();
                sched_timer();
                break;
                
            case 9:
                break;
        }
    } else {
        switch (scause) {
            case 2:
                uart_puts("Illegal instruction at ");
                uart_puthex(sepc);
                uart_puts("\n");
                proc_exit(-1);
                break;
                
            case 8: {
                struct proc* p = proc_current();
                if (p && p->trapframe) {
                    uint64_t syscall_num = tf->a7;
                    
                    tf->a0 = syscall_dispatch(
                        syscall_num,
                        tf->a0, tf->a1, tf->a2,
                        tf->a3, tf->a4, tf->a5
                    );
                    
                    w_sepc(sepc + 4);
                }
                break;
            }
                
            case 12:
            case 13:
            case 15: {
                uint64_t stval = r_stval();
                uart_puts("Page fault at ");
                uart_puthex(sepc);
                uart_puts(" accessing ");
                uart_puthex(stval);
                uart_puts("\n");
                proc_exit(-1);
                break;
            }
                
            default:
                uart_puts("Unknown trap ");
                uart_putnum(scause, 10);
                uart_puts(" at ");
                uart_puthex(sepc);
                uart_puts("\n");
                proc_exit(-1);
                break;
        }
    }
}
