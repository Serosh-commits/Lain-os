#include <stdint.h>
#include <stddef.h>

namespace abyss {

void* kmalloc(size_t size);

struct FileNode {
    char name[32];
    uint8_t* data;
    size_t size;
    size_t capacity;
    bool is_dir;
    FileNode* parent;
    FileNode* children;
    FileNode* next;
};

class NeuroVFS {
    uint8_t global_entropy = 0;
    FileNode* root = nullptr;
    FileNode* cwd = nullptr;

    void strcpy(char* dst, const char* src) {
        while (*src) *dst++ = *src++;
        *dst = 0;
    }

    int strcmp(const char* s1, const char* s2) {
        while (*s1 && (*s1 == *s2)) { s1++; s2++; }
        return *(const unsigned char*)s1 - *(const unsigned char*)s2;
    }

    FileNode* find_child(FileNode* dir, const char* name) {
        if (!dir || !dir->is_dir) return nullptr;
        FileNode* cur = dir->children;
        while (cur) {
            if (strcmp(cur->name, name) == 0) return cur;
            cur = cur->next;
        }
        return nullptr;
    }

public:
    void init() {
        root = (FileNode*)kmalloc(sizeof(FileNode));
        strcpy(root->name, "/");
        root->is_dir = true;
        root->parent = root;
        root->children = nullptr;
        root->next = nullptr;
        
        cwd = root;

        mkdir("home");
        change_dir("home");
        mkdir("lain");
        change_dir("lain");
        
        create_file("welcome.txt");
        write("welcome.txt", "Welcome to the Wired.", 21);
    }

    FileNode* get_cwd() { return cwd; }

    bool mkdir(const char* name) {
        if (find_child(cwd, name)) return false;

        FileNode* node = (FileNode*)kmalloc(sizeof(FileNode));
        if (!node) return false;

        strcpy(node->name, name);
        node->data = nullptr;
        node->size = 0;
        node->is_dir = true;
        node->parent = cwd;
        node->children = nullptr;
        node->next = cwd->children;
        cwd->children = node;
        return true;
    }

    bool create_file(const char* name) {
        if (find_child(cwd, name)) return false;

        FileNode* node = (FileNode*)kmalloc(sizeof(FileNode));
        if (!node) return false;

        strcpy(node->name, name);
        node->data = nullptr;
        node->size = 0;
        node->capacity = 0;
        node->is_dir = false;
        node->parent = cwd;
        node->next = cwd->children;
        cwd->children = node;
        return true;
    }

    bool change_dir(const char* path) {
        if (strcmp(path, "/") == 0) {
            cwd = root;
            return true;
        }
        if (strcmp(path, "..") == 0) {
            cwd = cwd->parent;
            return true;
        }
        if (strcmp(path, ".") == 0) return true;

        FileNode* target = find_child(cwd, path);
        if (target && target->is_dir) {
            cwd = target;
            return true;
        }
        return false;
    }

    void pwd() {
        volatile char* uart = (volatile char*)0x10000000;
        print_path_recursive(cwd, uart);
        if (cwd == root) *uart = '/';
        *uart = '\n';
    }

    void print_path_recursive(FileNode* node, volatile char* uart) {
        if (node == root) return;
        print_path_recursive(node->parent, uart);
        *uart = '/';
        const char* n = node->name;
        while(*n) *uart = *n++;
    }

    bool write(const char* name, const char* buffer, size_t len) {
        FileNode* node = find_child(cwd, name);
        if (!node || node->is_dir) return false;

        if (len > node->capacity) {
            node->data = (uint8_t*)kmalloc(len + 1);
            node->capacity = len;
        }
        const uint8_t* buf = (const uint8_t*)buffer;
        for (size_t i = 0; i < len; ++i) node->data[i] = buf[i];
        node->size = len;
        return true;
    }

    int read(const char* name, char* buffer, size_t max_len) {
        FileNode* node = find_child(cwd, name);
        if (!node || node->is_dir) return -1;

        size_t to_read = (node->size < max_len) ? node->size : max_len;
        for (size_t i = 0; i < to_read; ++i) buffer[i] = node->data[i];
        buffer[to_read] = 0;
        return to_read;
    }

    void list() {
        volatile char* uart = (volatile char*)0x10000000;
        FileNode* cur = cwd->children;
        const char* hdr = "TYPE    SIZE  NAME\n";
        while(*hdr) *uart = *hdr++;
        
        while (cur) {
            const char* type = cur->is_dir ? "DIR " : "FILE";
            while(*type) *uart = *type++;
            *uart = ' '; *uart = ' '; *uart = ' '; *uart = ' ';

            char nums[32];
            int n = cur->size; 
            int idx = 0;
            if (n==0) nums[idx++]='0';
            while(n>0) { nums[idx++] = (n%10)+'0'; n/=10; }
            for(int i=0; i<6-idx; ++i) *uart = ' ';
            while(idx>0) *uart = nums[--idx];
            
            *uart = ' '; *uart = ' ';
            const char* nm = cur->name;
            while(*nm) *uart = *nm++;
            *uart = '\n';
            cur = cur->next;
        }
    }
    
    void remove_recursive(FileNode* node) {
         if (!node) return;
         if (node->is_dir) {
             FileNode* child = node->children;
             while (child) {
                 FileNode* next = child->next;
                 remove_recursive(child);
                 child = next;
             }
         }
    }

    bool copy_file(const char* src_name, const char* dst_name) {
        FileNode* src = find_child(cwd, src_name);
        if (!src || src->is_dir) return false;
        if (!create_file(dst_name)) return false;
        FileNode* dst = find_child(cwd, dst_name);
        if (!dst) return false;
        if (src->size > 0) {
            dst->data = (uint8_t*)kmalloc(src->size + 1);
            for (size_t i = 0; i < src->size; ++i) dst->data[i] = src->data[i];
            dst->size = src->size;
            dst->capacity = src->size;
        }
        return true;
    }

    bool rename_file(const char* old_name, const char* new_name) {
        FileNode* node = find_child(cwd, old_name);
        if (!node) return false;
        if (find_child(cwd, new_name)) return false;
        strcpy(node->name, new_name);
        return true;
    }

    bool remove(const char* name) {
        FileNode* cur = cwd->children;
        FileNode* prev = nullptr;
        while(cur) {
            if (strcmp(cur->name, name) == 0) {
                if (prev) prev->next = cur->next;
                else cwd->children = cur->next;
                
                remove_recursive(cur);
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }
};
}
