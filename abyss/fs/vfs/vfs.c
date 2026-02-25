#include "fs/vfs.h"
#include "mm/kmalloc.h"
#include "lib/string.h"
#include "drivers/uart.h"

#define NINODES 128

static struct inode inodes[NINODES];
static int next_inum = 1;
static struct inode* root_inode;
static struct inode* cwd;

void vfs_init() {
    for (int i = 0; i < NINODES; i++) {
        inodes[i].type = FT_NONE;
        inodes[i].inum = 0;
        inodes[i].name[0] = 0;
        inodes[i].parent = NULL;
    }
    
    root_inode = &inodes[0];
    root_inode->inum = next_inum++;
    root_inode->type = FT_DIR;
    root_inode->nlink = 1;
    root_inode->size = 0;
    root_inode->data = NULL;
    root_inode->parent = root_inode;
    strcpy(root_inode->name, "/");
    
    cwd = root_inode;
}

static struct inode* inode_alloc(enum file_type type, struct inode* parent) {
    for (int i = 0; i < NINODES; i++) {
        if (inodes[i].type == FT_NONE) {
            inodes[i].inum = next_inum++;
            inodes[i].type = type;
            inodes[i].nlink = 1;
            inodes[i].size = 0;
            inodes[i].data = NULL;
            inodes[i].parent = parent;
            return &inodes[i];
        }
    }
    return NULL;
}

static struct inode* find_in_dir(struct inode* dir, const char* name) {
    if (!dir || dir->type != FT_DIR) return NULL;
    for (int i = 0; i < NINODES; i++) {
        if (inodes[i].type != FT_NONE && inodes[i].parent == dir) {
            if (strcmp(inodes[i].name, name) == 0) {
                return &inodes[i];
            }
        }
    }
    return NULL;
}

struct inode* vfs_namei(const char* path) {
    if (!path || path[0] == '\0') return NULL;
    
    struct inode* curr;
    if (path[0] == '/') {
        curr = root_inode;
        path++;
    } else {
        curr = cwd;
    }
    
    char part[MAX_NAME];
    while (*path) {
        int i = 0;
        while (*path && *path != '/') {
            if (i < MAX_NAME - 1) part[i++] = *path;
            path++;
        }
        part[i] = '\0';
        
        if (i > 0) {
            if (strcmp(part, ".") == 0) {
            } else if (strcmp(part, "..") == 0) {
                curr = curr->parent;
            } else {
                curr = find_in_dir(curr, part);
                if (!curr) return NULL;
            }
        }
        
        if (*path == '/') path++;
    }
    
    return curr;
}

struct file* vfs_open(const char* path, int flags) {
    struct inode* inode = vfs_namei(path);
    
    if (!inode) {
        if (flags & 0x200) {
            char parent_path[MAX_PATH];
            char file_name[MAX_NAME];
            strcpy(parent_path, path);
            char* last_slash = NULL;
            for(int i = 0; i < strlen(parent_path); i++) if(parent_path[i] == '/') last_slash = &parent_path[i];
            struct inode* parent;
            
            if (!last_slash) {
                parent = cwd;
                strcpy(file_name, path);
            } else {
                if (last_slash == parent_path) {
                    parent = root_inode;
                } else {
                    *last_slash = '\0';
                    parent = vfs_namei(parent_path);
                }
                strcpy(file_name, last_slash + 1);
            }
            
            if (parent && parent->type == FT_DIR) {
                inode = inode_alloc(FT_FILE, parent);
                if (inode) strcpy(inode->name, file_name);
            }
        }
    }
    
    if (!inode) return NULL;
    
    struct file* f = kmalloc(sizeof(struct file));
    if (!f) return NULL;
    
    f->inode = inode;
    f->offset = 0;
    f->flags = flags;
    f->refcount = 1;
    
    return f;
}

struct file* vfs_file_dup(struct file* f) {
    if (f) f->refcount++;
    return f;
}

void vfs_close(struct file* f) {
    if (f) {
        f->refcount--;
        if (f->refcount <= 0) kfree(f);
    }
}

int vfs_read(struct file* f, char* buf, size_t count) {
    if (!f || !f->inode || f->inode->type != FT_FILE) return -1;
    if (f->offset >= f->inode->size) return 0;
    
    size_t remaining = f->inode->size - f->offset;
    if (count > remaining) count = remaining;
    
    if (f->inode->data) memcpy(buf, f->inode->data + f->offset, count);
    else memset(buf, 0, count);
    
    f->offset += count;
    return count;
}

int vfs_write(struct file* f, const char* buf, size_t count) {
    if (!f || !f->inode || f->inode->type != FT_FILE) return -1;
    
    if (!f->inode->data) {
        f->inode->data = kmalloc(4096);
        if (!f->inode->data) return -1;
        f->inode->size = 0;
    }
    
    size_t new_size = f->offset + count;
    if (new_size > 4096) {
        count = 4096 - f->offset;
        new_size = 4096;
    }
    
    memcpy(f->inode->data + f->offset, buf, count);
    f->offset += count;
    if (new_size > f->inode->size) f->inode->size = new_size;
    
    return count;
}

int vfs_mkdir(const char* path) {
    if (vfs_namei(path)) return -1;
    
    char parent_path[MAX_PATH];
    char dir_name[MAX_NAME];
    strcpy(parent_path, path);
    char* last_slash = NULL;
    for(int i = 0; i < strlen(parent_path); i++) if(parent_path[i] == '/') last_slash = &parent_path[i];
    struct inode* parent;
    
    if (!last_slash) {
        parent = cwd;
        strcpy(dir_name, path);
    } else {
        if (last_slash == parent_path) parent = root_inode;
        else {
            *last_slash = '\0';
            parent = vfs_namei(parent_path);
        }
        strcpy(dir_name, last_slash + 1);
    }
    
    if (!parent || parent->type != FT_DIR) return -1;
    
    struct inode* inode = inode_alloc(FT_DIR, parent);
    if (!inode) return -1;
    strcpy(inode->name, dir_name);
    
    return 0;
}

int vfs_chdir(const char* path) {
    struct inode* inode = vfs_namei(path);
    if (!inode || inode->type != FT_DIR) return -1;
    cwd = inode;
    return 0;
}

int vfs_remove(const char* path) {
    struct inode* inode = vfs_namei(path);
    if (!inode || inode == root_inode) return -1;
    
    inode->type = FT_NONE;
    inode->inum = 0;
    inode->name[0] = 0;
    inode->parent = NULL;
    if (inode->data) {
        kfree(inode->data);
        inode->data = NULL;
    }
    return 0;
}

void vfs_ls() {
    uart_puts("   INUM  TYPE  SIZE    NAME\n");
    for (int i = 0; i < NINODES; i++) {
        if (inodes[i].type != FT_NONE && inodes[i].parent == cwd) {
            uart_putnum(inodes[i].inum, 10);
            uart_puts("     ");
            if (inodes[i].type == FT_DIR) uart_puts("DIR   ");
            else uart_puts("FILE  ");
            uart_putnum(inodes[i].size, 10);
            uart_puts("    ");
            uart_puts(inodes[i].name);
            uart_puts("\n");
        }
    }
}

void vfs_pwd(char* buf) {
    if (cwd) strcpy(buf, cwd->name);
    else strcpy(buf, "/");
}
