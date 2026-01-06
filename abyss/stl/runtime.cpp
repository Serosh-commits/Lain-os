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

namespace abyss {
void itoa(char* buf, uint64_t n) {
    if (n == 0) { buf[0] = '0'; buf[1] = 0; return; }
    int i = 0;
    while (n > 0) { buf[i++] = (n % 10) + '0'; n /= 10; }
    buf[i] = 0;
    for (int j = 0; j < i / 2; ++j) {
        char tmp = buf[j];
        buf[j] = buf[i - j - 1];
        buf[i - j - 1] = tmp;
    }
}
}
