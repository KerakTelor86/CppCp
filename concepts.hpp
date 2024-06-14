#ifndef CPPCP_CONCEPTS
#define CPPCP_CONCEPTS

#include <concepts>

#include "ostream.hpp"
#include "types.hpp"

namespace CppCp {

template <typename T>
concept IStreamCompatible = requires(T& it, std::istream& stream) {
    { stream >> it } -> std::same_as<std::istream&>;
};

template <typename... T>
concept IStreamCompatibleAll = requires {
    requires((IStreamCompatible<T> && ...));
};

template <typename T>
concept OStreamCompatible = requires(const T& it, std::ostream& stream) {
    { stream << it } -> std::same_as<std::ostream&>;
};

template <typename... T>
concept OStreamCompatibleAll = requires {
    requires((OStreamCompatible<T>) && ...);
};

template <typename T>
concept IndexableContainer = requires(const T& container, usize idx) {
    { container[idx] };
};

template <typename V, typename T>
concept IndexableContainerOf = requires(const V& container, usize idx) {
    { container[idx] } -> std::same_as<T>;
};
template <typename Func, typename... Args>
concept Lambda = requires(const Func& func, const Args&... args) {
    { func(args...) };
};

template <typename Ret, typename Func, typename... Args>
concept LambdaWithRet = requires(const Func& func, const Args&... args) {
    { func(args...) } -> std::same_as<Ret>;
};

} // namespace CppCp

#endif
