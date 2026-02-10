#ifndef PROCESS_H
#define PROCESS_H

#include "kernel/riscv.h"
#include "lib/types.h"

#define NPROC 64
#define NOFILE 16
#define KSTACK_SIZE 4096

enum procstate {
    UNUSED,
    SLEEPING,
    RUNNABLE,
    RUNNING,
    ZOMBIE
};

struct context {
    uint64_t ra;
    uint64_t sp;
    uint64_t s0;
    uint64_t s1;
    uint64_t s2;
    uint64_t s3;
    uint64_t s4;
    uint64_t s5;
    uint64_t s6;
    uint64_t s7;
    uint64_t s8;
    uint64_t s9;
    uint64_t s10;
    uint64_t s11;
};

struct trapframe {
    uint64_t ra;
    uint64_t sp;
    uint64_t gp;
    uint64_t tp;
    uint64_t t0;
    uint64_t t1;
    uint64_t t2;
    uint64_t s0;
    uint64_t s1;
    uint64_t a0;
    uint64_t a1;
    uint64_t a2;
    uint64_t a3;
    uint64_t a4;
    uint64_t a5;
    uint64_t a6;
    uint64_t a7;
    uint64_t s2;
    uint64_t s3;
    uint64_t s4;
    uint64_t s5;
    uint64_t s6;
    uint64_t s7;
    uint64_t s8;
    uint64_t s9;
    uint64_t s10;
    uint64_t s11;
    uint64_t t3;
    uint64_t t4;
    uint64_t t5;
    uint64_t t6;
};

struct file;

struct proc {
    int pid;
    enum procstate state;
    struct proc* parent;
    void* chan;
    int killed;
    int exit_status;
    char name[16];
    
    uint64_t kstack;
    pagetable_t pagetable;
    struct trapframe* trapframe;
    struct context context;
    
    uint64_t sz;
    struct file* ofile[NOFILE];
    
    uint64_t timeline_id;
    uint64_t priority;
};

void proc_init();
struct proc* proc_alloc();
void proc_free(struct proc* p);
struct proc* proc_current();
void proc_set_current(struct proc* p);
int proc_fork();
void proc_exit(int status);
int proc_wait(int* status);
int proc_kill(int pid);
void proc_sleep(void* chan);
void proc_wakeup(void* chan);

extern struct proc proc_table[NPROC];

#endif
