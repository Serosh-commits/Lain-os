#pragma once
#include "utility.hpp"
#include <stddef.h>
void* operator new(size_t size);
void operator delete(void* ptr) noexcept;
void operator delete(void* ptr, size_t size) noexcept;

namespace std {

template<typename T>
class vector {
    T* data_;
    size_t size_;
    size_t capacity_;

public:
    using value_type = T;
    using size_type = size_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    using iterator = T*;
    using const_iterator = const T*;

    vector() : data_(nullptr), size_(0), capacity_(0) {}
    
    ~vector() { clear(); ::operator delete(data_); }

    void push_back(const T& value) {
        if (size_ == capacity_) reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        new (&data_[size_++]) T(value);
    }

    void push_back(T&& value) {
        if (size_ == capacity_) reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        new (&data_[size_++]) T(move(value));
    }

    void pop_back() {
        if (size_ > 0) {
            data_[--size_].~T();
        }
    }

    void reserve(size_t new_cap) {
        if (new_cap <= capacity_) return;
        T* new_data = (T*)::operator new(new_cap * sizeof(T));
        if (data_) {
            for (size_t i = 0; i < size_; ++i) {
                new (&new_data[i]) T(move(data_[i]));
                data_[i].~T();
            }
            ::operator delete(data_);
        }
        data_ = new_data;
        capacity_ = new_cap;
    }

    void clear() {
        for (size_t i = 0; i < size_; ++i) data_[i].~T();
        size_ = 0;
    }

    T& operator[](size_t index) { return data_[index]; }
    const T& operator[](size_t index) const { return data_[index]; }
    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    T* begin() { return data_; }
    T* end() { return data_ + size_; }
    const T* begin() const { return data_; }
    const T* end() const { return data_ + size_; }
    T& back() { return data_[size_ - 1]; }
    T& front() { return data_[0]; }
};

}

inline void* operator new(size_t, void* p) noexcept { return p; }
