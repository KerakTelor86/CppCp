#ifndef CPPCP_DEBUG
#define CPPCP_DEBUG

#include <iostream>
#include <sstream>

#include "concepts.hpp"
#include "io.hpp"

#ifdef LOCAL

#define debug(...) __VA_ARGS__;
#define with_name(x)                                                           \
    [&x] {                                                                     \
        std::stringstream ss;                                                  \
        ss << "[" << #x << " -> " << x << "]";                                 \
        return ss.str();                                                       \
    }()

#else

#define debug(...)
#define with_name(x) x

#endif

namespace CppCp {

#if __cplusplus >= 202002L
constexpr string COLOR_GREEN = "\u001b[32m";
constexpr string COLOR_RESET = "\u001b[0m";
#else
const string COLOR_GREEN = "\u001b[32m";
const string COLOR_RESET = "\u001b[0m";
#endif

#if __cplusplus >= 202002L
template <bool add_space = true, OStreamCompatibleAll... T>
#else
template <bool add_space = true, typename... T>
#endif
inline void write_debug([[maybe_unused]] T... args) {
#ifdef LOCAL
    write(COLOR_GREEN);
    write<add_space>(args...);
    write(COLOR_RESET);
#endif
}

#if __cplusplus >= 202002L
template <bool add_space = true, OStreamCompatibleAll... T>
#else
template <bool add_space = true, typename... T>
#endif
inline void write_line_debug([[maybe_unused]] T... args) {
#ifdef LOCAL
    write_debug<add_space>(args...);
    write_debug('\n');
#endif
}

#define write_format_debug(...) write_debug(std::format(__VA_ARGS__))

} // namespace CppCp

#endif
