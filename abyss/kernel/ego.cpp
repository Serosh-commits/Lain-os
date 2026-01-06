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
    uint64_t syscall_unshare_ego(uint64_t pid) {
        if (ego_count >= MAX_EGOS) return -1;
        int nid = ego_count++;
        egos[nid].ego_id = nid;
        egos[nid].visible_pids.set(pid);
        return nid;
    }
    bool can_see(uint64_t eid, uint64_t pid) {
        if (eid >= MAX_EGOS) return false;
        return (pid == 0) || egos[eid].visible_pids.get(pid);
    }
    void merge(uint64_t src, uint64_t dst) {
        if (src >= MAX_EGOS || dst >= MAX_EGOS) return;
        egos[dst].visible_pids.merge(egos[src].visible_pids);
    }
};
}
