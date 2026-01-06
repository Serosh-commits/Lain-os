#pragma once
#include <stddef.h>
#include "utility.hpp"

namespace std {

constexpr size_t dynamic_extent = -1;

template<class T, size_t Extent = dynamic_extent>
class span {
public:
    using element_type = T;
    using value_type = remove_cv_t<T>;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    constexpr span() : data_(nullptr), size_(0) {}
    constexpr span(T* ptr, size_t count) : data_(ptr), size_(count) {}
    template<size_t N>
    size_type size_;
};

} // namespace std
