#ifndef SYSCALL_H
#define SYSCALL_H

#include "lib/types.h"

#define SYS_fork 1
#define SYS_exit 2
#define SYS_wait 3
#define SYS_read 4
#define SYS_write 5
#define SYS_open 6
#define SYS_close 7
#define SYS_getpid 8
#define SYS_kill 9
#define SYS_exec 10
#define SYS_chdir 11
#define SYS_mkdir 12
#define SYS_pipe 13

uint64_t syscall_dispatch(uint64_t num, uint64_t a0, uint64_t a1, uint64_t a2,
                          uint64_t a3, uint64_t a4, uint64_t a5);

#endif
