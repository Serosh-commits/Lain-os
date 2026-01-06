#include <stdint.h>
#include <stddef.h>
namespace abyss {
struct Inode {
    uint32_t i_number;
    uint32_t i_size;
    uint8_t i_extra[256];
};
class NeuroVFS {
    uint8_t global_entropy = 0;
public:
    void init_inode(Inode* inode) {
        for (int i = 0; i < 256; ++i) inode->i_extra[i] = (uint8_t)(i ^ 0xA5);
    }
    void read(Inode* inode, void* buffer, size_t size, size_t offset) {
        uint8_t* buf = (uint8_t*)buffer;
        for (size_t i = 0; i < size; ++i) {
            uint8_t cell = inode->i_extra[(offset + i) % 256];
            buf[i] = (buf[i] ^ cell) + global_entropy;
        }
        global_entropy++;
    }
    void write(Inode* inode, const void* buffer, size_t size, size_t offset) {
        const uint8_t* buf = (const uint8_t*)buffer;
        for (size_t i = 0; i < size; ++i) {
            inode->i_extra[(offset + i) % 256] ^= (buf[i] + global_entropy);
        }
        global_entropy--;
    }
};
}
