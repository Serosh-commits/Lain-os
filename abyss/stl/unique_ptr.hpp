#pragma once
#include "utility.hpp"

namespace std {

template<typename T>
struct default_delete {
    void operator()(T* ptr) const { delete ptr; }
};

template<typename T, typename D = default_delete<T>>
class unique_ptr {
    T* ptr_;
    D deleter_;
public:
    explicit unique_ptr(T* p = nullptr) : ptr_(p) {}
    ~unique_ptr() { if (ptr_) deleter_(ptr_); }
    unique_ptr(unique_ptr&& other) noexcept : ptr_(other.ptr_) { other.ptr_ = nullptr; }
    unique_ptr& operator=(unique_ptr&& other) noexcept {
        if (this != &other) {
            reset(other.ptr_);
            other.ptr_ = nullptr;
        }
        return *this;
    }
    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;
    T* get() const { return ptr_; }
    T* operator->() const { return ptr_; }
    T& operator*() const { return *ptr_; }
    explicit operator bool() const { return ptr_ != nullptr; }
    void reset(T* p = nullptr) {
        if (ptr_) deleter_(ptr_);
        ptr_ = p;
    }
    T* release() {
        T* p = ptr_;
        ptr_ = nullptr;
        return p;
    }

private:
    pointer ptr_;
    [[no_unique_address]] Deleter deleter_;
};

template<class T, class... Args>
unique_ptr<T> make_unique(Args&&... args) {
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

}
