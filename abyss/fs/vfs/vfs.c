#include "fs/vfs.h"
#include "mm/kmalloc.h"
#include "lib/string.h"

#define NINODES 128

static struct inode inodes[NINODES];
static int next_inum = 1;
static struct inode* root_inode;
static struct inode* cwd;

void vfs_init() {
    for (int i = 0; i < NINODES; i++) {
        inodes[i].type = FT_NONE;
        inodes[i].inum = 0;
    }
    
    root_inode = &inodes[0];
    root_inode->inum = next_inum++;
    root_inode->type = FT_DIR;
    root_inode->nlink = 1;
    root_inode->size = 0;
    root_inode->data = NULL;
    
    cwd = root_inode;
}

static struct inode* inode_alloc(enum file_type type) {
    for (int i = 0; i < NINODES; i++) {
        if (inodes[i].type == FT_NONE) {
            inodes[i].inum = next_inum++;
            inodes[i].type = type;
            inodes[i].nlink = 1;
            inodes[i].size = 0;
            inodes[i].data = NULL;
            return &inodes[i];
        }
    }
    return NULL;
}

struct inode* vfs_namei(const char* path) {
    if (strcmp(path, "/") == 0) {
        return root_inode;
    }
    
    if (strcmp(path, ".") == 0) {
        return cwd;
    }
    
    char name[MAX_NAME];
    int i = 0;
    while (*path && *path != '/' && i < MAX_NAME - 1) {
        name[i++] = *path++;
    }
    name[i] = 0;
    
    for (int j = 0; j < NINODES; j++) {
        if (inodes[j].type != FT_NONE) {
            if (strcmp((char*)&inodes[j].inum, name) == 0) {
                return &inodes[j];
            }
        }
    }
    
    return NULL;
}

struct file* vfs_open(const char* path, int flags) {
    struct inode* inode = vfs_namei(path);
    
    if (!inode) {
        if (flags & 0x200) {
            inode = inode_alloc(FT_FILE);
            if (!inode) {
                return NULL;
            }
        } else {
            return NULL;
        }
    }
    
    struct file* f = kmalloc(sizeof(struct file));
    if (!f) {
        return NULL;
    }
    
    f->inode = inode;
    f->offset = 0;
    f->flags = flags;
    
    return f;
}

void vfs_close(struct file* f) {
    if (f) {
        kfree(f);
    }
}

int vfs_read(struct file* f, char* buf, size_t count) {
    if (!f || !f->inode || f->inode->type != FT_FILE) {
        return -1;
    }
    
    if (f->offset >= f->inode->size) {
        return 0;
    }
    
    size_t remaining = f->inode->size - f->offset;
    if (count > remaining) {
        count = remaining;
    }
    
    if (f->inode->data) {
        memcpy(buf, f->inode->data + f->offset, count);
    } else {
        memset(buf, 0, count);
    }
    
    f->offset += count;
    return count;
}

int vfs_write(struct file* f, const char* buf, size_t count) {
    if (!f || !f->inode || f->inode->type != FT_FILE) {
        return -1;
    }
    
    if (!f->inode->data) {
        f->inode->data = kmalloc(4096);
        if (!f->inode->data) {
            return -1;
        }
        f->inode->size = 0;
    }
    
    size_t new_size = f->offset + count;
    if (new_size > 4096) {
        count = 4096 - f->offset;
        new_size = 4096;
    }
    
    memcpy(f->inode->data + f->offset, buf, count);
    f->offset += count;
    
    if (new_size > f->inode->size) {
        f->inode->size = new_size;
    }
    
    return count;
}

int vfs_mkdir(const char* path) {
    struct inode* inode = vfs_namei(path);
    if (inode) {
        return -1;
    }
    
    inode = inode_alloc(FT_DIR);
    if (!inode) {
        return -1;
    }
    
    return 0;
}

int vfs_chdir(const char* path) {
    struct inode* inode = vfs_namei(path);
    if (!inode || inode->type != FT_DIR) {
        return -1;
    }
    
    cwd = inode;
    return 0;
}
