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
    void* stack_copy; 
    uint64_t fpu_state[32]; 
    uint64_t timestamp;
    uint64_t probability;
};
struct Process {
    uint64_t pid;
    std::vector<Timeline> timelines;
    Process() { timelines.push_back(Timeline{}); }
};
constexpr int MAX_PROC = 64;
Process* process_table[MAX_PROC];
int proc_count = 0;
uint64_t ticks = 0;

class TemporalScheduler {
public:
    void init() {
        static Process kernel_process;
        kernel_process.pid = 0;
        process_table[0] = &kernel_process;
        proc_count = 1;
        current_process = &kernel_process;
    }
    void spawn(uint64_t pid) {
        if (proc_count < MAX_PROC) {
            Process* p = new Process();
            p->pid = pid;
            process_table[proc_count++] = p;
        }
    }
    void timer_interrupt() {
        ticks++;
        if (!current_process) return;
        for (auto& tl : current_process->timelines) {
            tl.timestamp++;
            if (tl.probability > 0) tl.probability--;
        }
    }
    void syscall_yield() {
        if (!current_process || current_process->timelines.size() >= 16) return;
        Timeline branch = current_process->timelines.back();
        branch.probability = 100;
        branch.timestamp++;
        current_process->timelines.push_back(branch);
    }
    void syscall_collapse(uint64_t tid) {
        if (!current_process || tid >= current_process->timelines.size()) return;
        Timeline winner = current_process->timelines[tid];
        current_process->timelines.clear();
        current_process->timelines.push_back(winner);
    }
    void schedule() {
        if (proc_count == 0) return;
        static int next_proc = 0;
        next_proc = (next_proc + 1) % proc_count;
        current_process = process_table[next_proc];
        if (current_process->timelines.empty()) return;
        size_t best = 0;
        for (size_t i = 1; i < current_process->timelines.size(); ++i) {
            if (current_process->timelines[i].probability > current_process->timelines[best].probability)
                best = i;
        }
        if (best != current_process->timelines.size() - 1) {
            Timeline tmp = current_process->timelines[best];
            current_process->timelines[best] = current_process->timelines.back();
            current_process->timelines.back() = tmp;
        }
    }
    void print_process_list() {
        volatile char* uart = (volatile char*)0x10000000;
        const char* hdr = "   PID  TIMELINES  STATUS\n";
        while (*hdr) *uart = *hdr++;

        for (int i = 0; i < proc_count; ++i) {
            Process* p = process_table[i];
            if (!p) continue;
            
            // Print PID
            char buf[32];
            int n = p->pid;
            int idx = 0;
            if (n==0) buf[idx++]='0';
            while(n>0) { buf[idx++]=(n%10)+'0'; n/=10; }
            for(int k=0; k<6-idx; ++k) *uart = ' ';
            while(idx>0) *uart = buf[--idx];

            *uart = ' '; *uart = ' '; *uart = ' '; *uart = ' '; *uart = ' '; *uart = ' ';

            // Print Timelines count
            n = p->timelines.size();
            idx = 0;
            if (n==0) buf[idx++]='0';
            while(n>0) { buf[idx++]=(n%10)+'0'; n/=10; }
            while(idx>0) *uart = buf[--idx];

            *uart = ' '; *uart = ' '; *uart = ' '; *uart = ' '; *uart = ' ';

            // Status
            const char* s = (p == current_process) ? "RUNNING" : "WAITING";
            while (*s) *uart = *s++;
            *uart = '\n';
        }
    }
    bool kill(uint64_t pid) {
        // Simple linear scan
        for (int i = 0; i < proc_count; ++i) {
             if (process_table[i] && process_table[i]->pid == pid) {
                 if (pid == 0) return false; // Can't kill kernel
                 process_table[i] = nullptr; // poor man's free
                 return true;
             }
        }
        return false;
    }
private:
    Process* current_process;
};
}
