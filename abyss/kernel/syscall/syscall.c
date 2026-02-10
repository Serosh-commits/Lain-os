#include "kernel/syscall.h"
#include "kernel/process.h"
#include "drivers/uart.h"
#include "fs/vfs.h"
#include "lib/string.h"

static uint64_t sys_fork() {
    return proc_fork();
}

static uint64_t sys_exit(int status) {
    proc_exit(status);
    return 0;
}

static uint64_t sys_wait(int* status) {
    return proc_wait(status);
}

static uint64_t sys_read(int fd, char* buf, size_t count) {
    if (fd == 0) {
        for (size_t i = 0; i < count; i++) {
            int c;
            while ((c = uart_getc()) == -1)
                ;
            buf[i] = c;
        }
        return count;
    }
    
    struct proc* p = proc_current();
    if (fd < 0 || fd >= NOFILE || !p->ofile[fd]) {
        return -1;
    }
    
    return vfs_read(p->ofile[fd], buf, count);
}

static uint64_t sys_write(int fd, const char* buf, size_t count) {
    if (fd == 1 || fd == 2) {
        for (size_t i = 0; i < count; i++) {
            uart_putc(buf[i]);
        }
        return count;
    }
    
    struct proc* p = proc_current();
    if (fd < 0 || fd >= NOFILE || !p->ofile[fd]) {
        return -1;
    }
    
    return vfs_write(p->ofile[fd], buf, count);
}

static uint64_t sys_open(const char* path, int flags) {
    struct proc* p = proc_current();
    
    struct file* f = vfs_open(path, flags);
    if (!f) {
        return -1;
    }
    
    for (int fd = 0; fd < NOFILE; fd++) {
        if (p->ofile[fd] == NULL) {
            p->ofile[fd] = f;
            return fd;
        }
    }
    
    return -1;
}

static uint64_t sys_close(int fd) {
    struct proc* p = proc_current();
    
    if (fd < 0 || fd >= NOFILE || !p->ofile[fd]) {
        return -1;
    }
    
    vfs_close(p->ofile[fd]);
    p->ofile[fd] = NULL;
    return 0;
}

static uint64_t sys_getpid() {
    struct proc* p = proc_current();
    return p ? p->pid : -1;
}

static uint64_t sys_kill(int pid) {
    return proc_kill(pid);
}

static uint64_t sys_chdir(const char* path) {
    return vfs_chdir(path);
}

static uint64_t sys_mkdir(const char* path) {
    return vfs_mkdir(path);
}

uint64_t syscall_dispatch(uint64_t num, uint64_t a0, uint64_t a1, uint64_t a2,
                          uint64_t a3, uint64_t a4, uint64_t a5) {
    switch (num) {
        case SYS_fork:
            return sys_fork();
        case SYS_exit:
            return sys_exit((int)a0);
        case SYS_wait:
            return sys_wait((int*)a0);
        case SYS_read:
            return sys_read((int)a0, (char*)a1, (size_t)a2);
        case SYS_write:
            return sys_write((int)a0, (const char*)a1, (size_t)a2);
        case SYS_open:
            return sys_open((const char*)a0, (int)a1);
        case SYS_close:
            return sys_close((int)a0);
        case SYS_getpid:
            return sys_getpid();
        case SYS_kill:
            return sys_kill((int)a0);
        case SYS_chdir:
            return sys_chdir((const char*)a0);
        case SYS_mkdir:
            return sys_mkdir((const char*)a0);
        default:
            return -1;
    }
}
