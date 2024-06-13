#ifndef CPPCP_CONCEPTS
#define CPPCP_CONCEPTS

#if __cplusplus >= 202002L
#include <concepts>
#endif

#include "ostream.hpp"
#include "types.hpp"

#if __cplusplus >= 202002L
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

} // namespace CppCp
#endif

#endif
