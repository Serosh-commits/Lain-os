#include "kernel/process.h"
#include "kernel/scheduler.h"
#include "mm/pmm.h"
#include "mm/vmm.h"
#include "lib/string.h"
#include "fs/vfs.h"

struct proc proc_table[NPROC];
static struct proc* current_proc;
static int next_pid = 1;

extern void context_switch(struct context* old, struct context* new_ctx);

void proc_init() {
    for (int i = 0; i < NPROC; i++) {
        proc_table[i].state = UNUSED;
        proc_table[i].pid = 0;
    }
    current_proc = NULL;
}

void forkret();

struct proc* proc_alloc() {
    for (int i = 0; i < NPROC; i++) {
        struct proc* p = &proc_table[i];
        if (p->state == UNUSED) {
            p->pid = next_pid++;
            p->state = RUNNABLE;
            p->parent = NULL;
            p->killed = 0;
            p->exit_status = 0;
            p->chan = NULL;
            p->timeline_id = 1;
            p->priority = 100;
            
            p->kstack = (uint64_t)pmm_alloc();
            if (!p->kstack) {
                p->state = UNUSED;
                return NULL;
            }
            
            p->pagetable = vmm_create();
            if (!p->pagetable) {
                pmm_free((void*)p->kstack);
                p->state = UNUSED;
                return NULL;
            }
            
            p->trapframe = pmm_alloc();
            if (!p->trapframe) {
                vmm_free(p->pagetable, 2);
                pmm_free((void*)p->kstack);
                p->state = UNUSED;
                return NULL;
            }
            
            memset(&p->context, 0, sizeof(p->context));
            p->context.ra = (uint64_t)forkret;
            p->context.sp = p->kstack + KSTACK_SIZE;
            
            for (int j = 0; j < NOFILE; j++) {
                p->ofile[j] = NULL;
            }
            
            return p;
        }
    }
    return NULL;
}

void forkret() {
    struct proc* p = proc_current();
    asm volatile("mv sp, %0" : : "r"(p->trapframe));
    asm volatile("j trapret");
}

void proc_free(struct proc* p) {
    if (p->trapframe) {
        pmm_free(p->trapframe);
    }
    if (p->pagetable) {
        vmm_free(p->pagetable, 2);
    }
    if (p->kstack) {
        pmm_free((void*)p->kstack);
    }
    
    p->pid = 0;
    p->parent = NULL;
    p->name[0] = 0;
    p->chan = NULL;
    p->killed = 0;
    p->state = UNUSED;
}

struct proc* proc_current() {
    return current_proc;
}

void proc_set_current(struct proc* p) {
    current_proc = p;
}

int proc_fork() {
    struct proc* np = proc_alloc();
    if (!np) {
        return -1;
    }
    
    struct proc* p = proc_current();
    
    if (p->sz > 0) {
        for (uint64_t i = 0; i < p->sz; i += PGSIZE) {
            uint64_t pa = vmm_translate(p->pagetable, i);
            if (pa == 0) {
                continue;
            }
            
            char* mem = pmm_alloc();
            if (!mem) {
                proc_free(np);
                return -1;
            }
            
            memmove(mem, (char*)pa, PGSIZE);
            vmm_map(np->pagetable, i, (uint64_t)mem, PGSIZE, PTE_R | PTE_W | PTE_X | PTE_U);
        }
    }
    
    np->sz = p->sz;
    np->parent = p;
    *np->trapframe = *p->trapframe;
    np->trapframe->a0 = 0;
    
    strcpy(np->name, p->name);
    
    for (int i = 0; i < NOFILE; i++) {
        if (p->ofile[i]) {
            np->ofile[i] = vfs_file_dup(p->ofile[i]);
        }
    }
    
    np->state = RUNNABLE;
    
    return np->pid;
}

void proc_exit(int status) {
    struct proc* p = proc_current();
    
    if (p == NULL) {
        return;
    }
    
    for (int fd = 0; fd < NOFILE; fd++) {
        if (p->ofile[fd]) {
            vfs_close(p->ofile[fd]);
            p->ofile[fd] = NULL;
        }
    }
    
    proc_wakeup(p->parent);
    
    for (int i = 0; i < NPROC; i++) {
        if (proc_table[i].parent == p) {
            proc_table[i].parent = NULL;
        }
    }
    
    p->exit_status = status;
    p->state = ZOMBIE;
    
    sched_yield();
}

int proc_wait(int* status) {
    struct proc* p = proc_current();
    
    for (;;) {
        int havekids = 0;
        
        for (int i = 0; i < NPROC; i++) {
            struct proc* np = &proc_table[i];
            if (np->parent == p) {
                havekids = 1;
                if (np->state == ZOMBIE) {
                    int pid = np->pid;
                    if (status) {
                        *status = np->exit_status;
                    }
                    proc_free(np);
                    return pid;
                }
            }
        }
        
        if (!havekids || p->killed) {
            return -1;
        }
        
        proc_sleep(p);
    }
}

int proc_kill(int pid) {
    for (int i = 0; i < NPROC; i++) {
        struct proc* p = &proc_table[i];
        if (p->pid == pid) {
            p->killed = 1;
            if (p->state == SLEEPING) {
                p->state = RUNNABLE;
            }
            return 0;
        }
    }
    return -1;
}

void proc_sleep(void* chan) {
    struct proc* p = proc_current();
    
    p->chan = chan;
    p->state = SLEEPING;
    
    sched_yield();
    
    p->chan = NULL;
}

void proc_wakeup(void* chan) {
    for (int i = 0; i < NPROC; i++) {
        struct proc* p = &proc_table[i];
        if (p->state == SLEEPING && p->chan == chan) {
            p->state = RUNNABLE;
        }
    }
}
