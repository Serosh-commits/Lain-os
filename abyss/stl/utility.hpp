namespace std {
template<typename T> struct remove_reference { using type = T; };
template<typename T> struct remove_reference<T&> { using type = T; };
template<typename T> struct remove_reference<T&&> { using type = T; };
template<typename T>
constexpr T&& forward(typename remove_reference<T>::type& t) noexcept { return static_cast<T&&>(t); }
template<typename T>
constexpr T&& forward(typename remove_reference<T>::type&& t) noexcept { return static_cast<T&&>(t); }
template<typename T>
constexpr typename remove_reference<T>::type&& move(T&& t) noexcept { return static_cast<typename remove_reference<T>::type&&>(t); }
template<typename T>
void swap(T& a, T& b) { T tmp = move(a); a = move(b); b = move(tmp); }
}
