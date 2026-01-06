#pragma once
#include "utility.hpp"
namespace std {
struct nullopt_t {};
inline constexpr nullopt_t nullopt{};
template<class T>
class optional {
    T value_;
    bool has_value_;
public:
    constexpr optional() noexcept : has_value_(false) {}
    constexpr optional(nullopt_t) noexcept : has_value_(false) {}
    constexpr optional(const T& value) : value_(value), has_value_(true) {}
    constexpr optional(T&& value) : value_(std::move(value)), has_value_(true) {}
    constexpr bool has_value() const noexcept { return has_value_; }
    constexpr explicit operator bool() const noexcept { return has_value_; }
    constexpr T& value() & { return value_; }
    constexpr const T& value() const & { return value_; }
    constexpr T& operator*() & { return value_; }
    constexpr const T& operator*() const & { return value_; }
    constexpr T* operator->() { return &value_; }
    constexpr const T* operator->() const { return &value_; }
    constexpr T value_or(T&& default_value) const& {
        return has_value_ ? value_ : static_cast<T>(std::forward<T>(default_value));
    }
};
}
