#include <stddef.h>
extern "C" {
    void* memset(void* ptr, int value, size_t num) {
        unsigned char* p = (unsigned char*)ptr;
        while (num--) *p++ = (unsigned char)value;
        return ptr;
    }
    void* memcpy(void* dest, const void* src, size_t num) {
        unsigned char* d = (unsigned char*)dest;
        const unsigned char* s = (const unsigned char*)src;
        while (num--) *d++ = *s++;
        return dest;
    }
    void* memmove(void* dest, const void* src, size_t num) {
        unsigned char* d = (unsigned char*)dest;
        const unsigned char* s = (const unsigned char*)src;
        if (d < s) {
            while (num--) *d++ = *s++;
        } else {
            d += num; s += num;
            while (num--) *--d = *--s;
        }
        return dest;
    }
    int atexit(void (*)(void)) { return 0; }
}
void* __dso_handle = 0;
