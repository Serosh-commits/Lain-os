#include <stdint.h>
#include <stddef.h>
namespace abyss {
struct Inode {
    uint32_t i_mode;
    uint32_t i_uid;
    uint32_t i_size;
    uint32_t i_atime;
    uint32_t i_ctime;
    uint32_t i_mtime;
    uint32_t i_dtime;
    uint16_t i_gid;
    uint16_t i_links_count;
    uint32_t i_blocks;
    uint32_t i_flags;
    uint32_t i_osd1;
    uint32_t i_block[15];
    uint32_t i_generation;
    uint32_t i_file_acl;
    uint32_t i_dir_acl;
    uint32_t i_faddr;
    uint8_t  i_osd2[12];
    
    // Extra 256 bytes for NeuroVFS
    uint8_t  i_extra[256];
};

class NeuroVFS {
public:
    void init_inode(Inode* inode) {
        for (int i = 0; i < 256; ++i) inode->i_extra[i] = (uint8_t)(i ^ 0x42);
    }
    void read(Inode* inode, void* buffer, size_t size, size_t offset) {
        uint8_t* buf = (uint8_t*)buffer;
        for (size_t i = 0; i < size; ++i) buf[i] ^= inode->i_extra[(offset + i) % 256];
    }
    void write(Inode* inode, const void* buffer, size_t size, size_t offset) {
        const uint8_t* buf = (const uint8_t*)buffer;
        for (size_t i = 0; i < size; ++i) inode->i_extra[(offset + i) % 256] ^= buf[i];
    }
};

} // namespace abyss
