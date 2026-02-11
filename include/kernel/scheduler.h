#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "lib/types.h"

struct timeline {
    uint64_t id;
    uint64_t priority;
    uint64_t quantum;
    uint64_t elapsed;
    struct timeline* next;
};

void sched_init();
void sched_yield();
void sched_timer();
void sched_ret();
struct timeline* timeline_create(uint64_t priority);
void timeline_assign(int pid, uint64_t timeline_id);
void sched_branch();
void sched_collapse(uint64_t id);

#endif
