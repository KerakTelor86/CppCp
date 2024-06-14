#ifndef CPPCP_IO
#define CPPCP_IO

#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "concepts.hpp"
#include "types.hpp"

namespace CppCp {

namespace {

template <typename T, usize... Idx>
inline void read_tuple_helper(T& tuple, std::index_sequence<Idx...>) {
    ((std::cin >> std::get<Idx>(tuple)), ...);
}

} // namespace

template <
    IStreamCompatible T,
    IStreamCompatible U,
    IStreamCompatibleAll... Rest>
inline std::tuple<T, U, Rest...> read() {
    std::tuple<T, U, Rest...> ret;
    read_tuple_helper(ret, std::index_sequence_for<T, U, Rest...>{});
    return ret;
}

template <IStreamCompatible T> inline T read() {
    T x;
    std::cin >> x;
    return x;
}

template <IStreamCompatible T, usize N> inline std::array<T, N> read() {
    std::array<T, N> x;
    for (auto& i : x) {
        i = read<T>();
    }
    return x;
}

template <
    IStreamCompatible T,
    IStreamCompatible U,
    IStreamCompatibleAll... Rest>
inline std::vector<std::tuple<T, U, Rest...>> read(const usize count) {
    std::vector<std::tuple<T, U, Rest...>> x(count);
    for (auto& i : x) {
        i = read<T, U, Rest...>();
    }
    return x;
}

template <IStreamCompatible T> inline std::vector<T> read(const usize count) {
    std::vector<T> x(count);
    for (auto& i : x) {
        i = read<T>();
    }
    return x;
}

inline string read_line() {
    string x;
    do {
        std::getline(std::cin, x);
    } while (x.length() == 0);
    return x;
}

template <bool add_space = true, OStreamCompatibleAll... T>
inline void write(const T... args) {
    if constexpr (add_space) {
        bool first = true;
        const auto output_space = [&]() {
            if (first) {
                first = false;
            } else {
                std::cout << ' ';
            }
        };
        ((output_space(), std::cout << args), ...);
    } else {
        ((std::cout << args), ...);
    }
}

template <bool add_space = true, OStreamCompatibleAll... T>
inline void write_line(const T... args) {
    write<add_space>(args...);
    write('\n');
}

#define write_format(...) write(std::format(__VA_ARGS__))

inline void flush() {
    std::cout.flush();
}

#ifndef LOCAL
struct FastIO {
    FastIO() {
        std::ios_base::sync_with_stdio(false);
        std::cin.tie(nullptr);
    }
} fast_io;
#endif

}; // namespace CppCp

#endif
