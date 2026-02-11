#include "kernel/scheduler.h"
#include "kernel/process.h"
#include "kernel/riscv.h"
#include "mm/kmalloc.h"
#include "lib/string.h"

static struct timeline* timelines;
static struct timeline* current_timeline;
static uint64_t next_timeline_id = 1;
static uint64_t global_ticks;

extern void context_switch(struct context* old, struct context* new_ctx);

void sched_init() {
    timelines = NULL;
    current_timeline = NULL;
    global_ticks = 0;
    
    struct timeline* default_tl = timeline_create(100);
    current_timeline = default_tl;
}

struct timeline* timeline_create(uint64_t priority) {
    struct timeline* tl = kmalloc(sizeof(struct timeline));
    if (!tl) {
        return NULL;
    }
    
    tl->id = next_timeline_id++;
    tl->priority = priority;
    tl->quantum = 100 * priority;
    tl->elapsed = 0;
    tl->next = timelines;
    timelines = tl;
    
    return tl;
}

void timeline_assign(int pid, uint64_t timeline_id) {
    for (int i = 0; i < NPROC; i++) {
        if (proc_table[i].pid == pid) {
            proc_table[i].timeline_id = timeline_id;
            
            struct timeline* tl = timelines;
            while (tl) {
                if (tl->id == timeline_id) {
                    proc_table[i].priority = tl->priority;
                    break;
                }
                tl = tl->next;
            }
            break;
        }
    }
}

static struct proc* pick_next() {
    struct proc* best = NULL;
    uint64_t best_priority = 0;
    
    for (int i = 0; i < NPROC; i++) {
        struct proc* p = &proc_table[i];
        if (p->state == RUNNABLE) {
            if (current_timeline && p->timeline_id == current_timeline->id) {
                if (p->priority > best_priority) {
                    best = p;
                    best_priority = p->priority;
                }
            } else if (!current_timeline) {
                if (p->priority > best_priority) {
                    best = p;
                    best_priority = p->priority;
                }
            }
        }
    }
    
    if (!best) {
        for (int i = 0; i < NPROC; i++) {
            struct proc* p = &proc_table[i];
            if (p->state == RUNNABLE) {
                if (p->priority > best_priority) {
                    best = p;
                    best_priority = p->priority;
                }
            }
        }
    }
    
    return best;
}

void sched_yield() {
    struct proc* p = proc_current();
    if (!p) {
        return;
    }
    
    struct proc* next = pick_next();
    if (!next || next == p) {
        return;
    }
    
    struct proc* old = p;
    proc_set_current(next);
    next->state = RUNNING;
    
    context_switch(&old->context, &next->context);
}

void sched_timer() {
    global_ticks++;
    
    if (current_timeline) {
        current_timeline->elapsed++;
        
        if (current_timeline->elapsed >= current_timeline->quantum) {
            current_timeline->elapsed = 0;
            current_timeline = current_timeline->next;
            if (!current_timeline) {
                current_timeline = timelines;
            }
        }
    }
    
    struct proc* p = proc_current();
    if (p && p->state == RUNNING) {
        p->state = RUNNABLE;
        sched_yield();
    }
}

void sched_ret() {
    struct proc* p = proc_current();
    if (p) {
        p->state = RUNNABLE;
    }
}

void sched_branch() {
    struct proc* p = proc_current();
    if (!p) return;
    
    struct timeline* tl = timeline_create(p->priority + 10);
    if (tl) {
        p->timeline_id = tl->id;
        current_timeline = tl;
    }
}

void sched_collapse(uint64_t id) {
    struct timeline* tl = timelines;
    struct timeline* prev = NULL;
    
    while (tl) {
        if (tl->id != id && tl->id != 1) { // Never collapse default timeline 1
            if (prev) prev->next = tl->next;
            else timelines = tl->next;
            
            struct timeline* tmp = tl;
            tl = tl->next;
            kfree(tmp);
        } else {
            if (tl->id == id) current_timeline = tl;
            prev = tl;
            tl = tl->next;
        }
    }
}
