#include <stdint.h>
#include <stddef.h>
namespace abyss {
class QuantumDriver {
    uint64_t violation_mask;
    bool entangled = false;
public:
    void init() { violation_mask = 0x55555555AAAAAAA; entangled = true; }
    void read_quantum1(void* buffer, size_t size) {
        if (!entangled) return;
        uint8_t* buf = (uint8_t*)buffer;
        for (size_t i = 0; i < size; ++i) buf[i] = (uint8_t)((violation_mask >> (i % 64)) & 0xFF);
    }
    void write_quantum0(const void* buffer, size_t size) {
        const uint8_t* buf = (const uint8_t*)buffer;
        for (size_t i = 0; i < size; ++i) {
            violation_mask ^= (uint64_t)buf[i] << (i % 64);
        }
        entangled = (violation_mask != 0);
    }
    bool is_entangled() const { return entangled; }
};
}
