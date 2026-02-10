#include "lib/types.h"

void* memset(void* dst, int c, size_t n) {
    char* cdst = (char*)dst;
    for (size_t i = 0; i < n; i++) {
        cdst[i] = c;
    }
    return dst;
}

void* memcpy(void* dst, const void* src, size_t n) {
    const char* s = src;
    char* d = dst;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dst;
}

void* memmove(void* dst, const void* src, size_t n) {
    const char* s = src;
    char* d = dst;
    
    if (s < d && s + n > d) {
        s += n;
        d += n;
        while (n-- > 0) {
            *--d = *--s;
        }
    } else {
        while (n-- > 0) {
            *d++ = *s++;
        }
    }
    return dst;
}

int memcmp(const void* v1, const void* v2, size_t n) {
    const uint8_t* s1 = v1;
    const uint8_t* s2 = v2;
    
    while (n-- > 0) {
        if (*s1 != *s2) {
            return *s1 - *s2;
        }
        s1++;
        s2++;
    }
    return 0;
}

size_t strlen(const char* s) {
    size_t n = 0;
    while (s[n]) {
        n++;
    }
    return n;
}

char* strcpy(char* dst, const char* src) {
    char* d = dst;
    while ((*d++ = *src++) != 0)
        ;
    return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
    char* d = dst;
    while (n > 0 && (*d = *src) != 0) {
        d++;
        src++;
        n--;
    }
    while (n > 0) {
        *d++ = 0;
        n--;
    }
    return dst;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const uint8_t*)s1 - *(const uint8_t*)s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    while (n > 0 && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) {
        return 0;
    }
    return *(const uint8_t*)s1 - *(const uint8_t*)s2;
}

char* strchr(const char* s, int c) {
    while (*s) {
        if (*s == c) {
            return (char*)s;
        }
        s++;
    }
    return NULL;
}

char* strcat(char* dst, const char* src) {
    char* d = dst;
    while (*d) {
        d++;
    }
    while ((*d++ = *src++) != 0)
        ;
    return dst;
}
