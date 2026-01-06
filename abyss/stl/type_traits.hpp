#pragma once

namespace std {
template<bool B, typename T = void> struct enable_if {};
template<typename T> struct enable_if<true, T> { using type = T; };

template<bool B, class T = void>
using enable_if_t = typename enable_if<B, T>::type;

template<typename T, typename U> struct is_same { static constexpr bool value = false; };
template<typename T> struct is_same<T, T> { static constexpr bool value = true; };

template<class T, class U>
constexpr bool is_same_v = is_same<T, U>::value;

template<typename T> struct remove_cv { using type = T; };
template<typename T> struct remove_cv<const T> { using type = T; };
template<typename T> struct remove_cv<volatile T> { using type = T; };
template<typename T> struct remove_cv<const volatile T> { using type = T; };

template<class T>
using remove_cv_t = typename remove_cv<T>::type;

}
