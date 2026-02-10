#ifndef VFS_H
#define VFS_H

#include "lib/types.h"

#define MAX_NAME 64
#define MAX_PATH 256

enum file_type {
    FT_NONE,
    FT_FILE,
    FT_DIR
};

struct inode {
    int inum;
    enum file_type type;
    int nlink;
    uint64_t size;
    char* data;
};

struct file {
    struct inode* inode;
    uint64_t offset;
    int flags;
};

struct dirent {
    int inum;
    char name[MAX_NAME];
};

void vfs_init();
struct file* vfs_open(const char* path, int flags);
void vfs_close(struct file* f);
int vfs_read(struct file* f, char* buf, size_t count);
int vfs_write(struct file* f, const char* buf, size_t count);
int vfs_mkdir(const char* path);
int vfs_chdir(const char* path);
struct inode* vfs_namei(const char* path);

#endif
