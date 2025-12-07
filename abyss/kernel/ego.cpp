#include <stdint.h>
#include <stddef.h>

namespace abyss {

struct BitMap {
    uint64_t bits[64];
    void set(int bit) { bits[bit / 64] |= (1ULL << (bit % 64)); }
    bool get(int bit) const { return (bits[bit / 64] >> (bit % 64)) & 1; }
    void merge(const BitMap& other) {
        for (int i = 0; i < 64; ++i) bits[i] |= other.bits[i];
    }
};
struct Ego { uint64_t ego_id; BitMap visible_pids; };
constexpr int MAX_EGOS = 4096;
Ego egos[MAX_EGOS];
int ego_count = 0;
class EgoManager {
public:
    void init() { egos[0].ego_id = 0; ego_count = 1; }
    uint64_t syscall_unshare_ego(uint64_t current_pid) {
        if (ego_count >= MAX_EGOS) return -1;
        int new_id = ego_count++;
        egos[new_id].ego_id = new_id;
        egos[new_id].visible_pids.set(current_pid);
        return new_id;
    }
    bool sched_visibility(uint64_t ego_id, uint64_t target_pid) {
        if (ego_id >= MAX_EGOS) return false;
        return egos[ego_id].visible_pids.get(target_pid);
    }
    void syscall_ego_merge(uint64_t current_ego, uint64_t target_ego) {
        if (current_ego >= MAX_EGOS || target_ego >= MAX_EGOS) return;
        egos[current_ego].visible_pids.merge(egos[target_ego].visible_pids);
    }
};

} // namespace abyss
