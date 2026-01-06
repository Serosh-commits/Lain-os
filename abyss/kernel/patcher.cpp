#include <stdint.h>
#include <stddef.h>

namespace abyss {

class Decompiler {
public:
    void syscall_decompile(uint64_t pid) {
        (void)pid;
    }
};
class HotPatcher {
public:
    void syscall_hotpatch(uint64_t pid, void* new_code, size_t size) {
        (void)pid; (void)new_code; (void)size;
    }
};

}
