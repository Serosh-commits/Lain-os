#pragma once
#include <stddef.h>
#include "utility.hpp"

namespace std {

class string_view {
public:
    constexpr string_view() : data_(nullptr), size_(0) {}
    constexpr string_view(const char* str, size_t count) : data_(str), size_(count) {}
    constexpr string_view(const char* str) : data_(str), size_(0) {
        if (str) while (str[size_]) size_++;
    }
    const_pointer data_;
    size_type size_;
};

}
