#include "../stl/vector.hpp"
#include <stdint.h>
namespace abyss {
struct Registers {
    uint64_t ra, sp, gp, tp, t0, t1, t2, s0, s1, a0, a1, a2, a3, a4, a5, a6, a7;
    uint64_t s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, t3, t4, t5, t6;
    uint64_t pc;
};

struct Timeline {
    Registers regs;
    // Stack copy would be a pointer to a cow page or deep copy
    void* stack_copy; 
    // FPU state
    uint64_t fpu_state[32]; 
    uint64_t timestamp;
    uint64_t probability; // 0-100
};

struct Process {
    uint64_t pid;
    std::vector<Timeline> timelines;
    Process() { timelines.push_back(Timeline{}); }
};
class TemporalScheduler {
public:
    void init() {}
    void timer_interrupt() {
        if (current_process && !current_process->timelines.empty()) {
            current_process->timelines.back().timestamp++;
        }
    }
    void syscall_yield() {
        if (current_process->timelines.size() < 10) {
            Timeline current = current_process->timelines.back();
            current.timestamp++;
            current_process->timelines.push_back(current);
        }
    }
    void syscall_collapse(uint64_t best_timeline_id) { (void)best_timeline_id; }
    void schedule() {
        if (current_process && current_process->timelines.size() > 1) {
            // Simple probability simulation
            if (current_process->timelines.back().timestamp % 2 == 0) {
                current_process->timelines.pop_back();
            }
        }
    }
private:
    Process* current_process;
};

} // namespace abyss
