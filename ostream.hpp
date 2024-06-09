#ifndef CPPCP_OSTREAM
#define CPPCP_OSTREAM

#include <ostream>

#include "types.hpp"

namespace CppCp {
template <typename T, typename U>
std::ostream& operator<<(
    std::ostream& stream, const std::pair<T, U>& pair
) {
    stream << pair.first << ' ' << pair.second << '\n';
    return stream;
}

template <typename T, std::size_t... Idx>
void ostream_tuple(std::ostream& stream, const T& tuple, std::index_sequence<Idx...>) {
    ((stream << (Idx == 0 ? "" : " ") << std::get<Idx>(tuple)),
     ...);
}

template <typename... T>
std::ostream& operator<<(
    std::ostream& stream, const std::tuple<T...>& tuple
) {
    ostream_tuple(stream, tuple, std::index_sequence_for<T...>{});
    return stream;
}

template <typename T>
std::ostream& operator<<(
    std::ostream& stream, const std::vector<T>& values
) {
    bool first = true;
    for (const auto& i : values) {
        if (first) {
            first = false;
        } else {
            stream << ' ';
        }
        stream << i;
    }
    return stream;
}

template <typename T, usize Size>
std::ostream& operator<<(
    std::ostream& stream, const std::array<T, Size>& values
) {
    bool first = true;
    for (const auto& i : values) {
        if (first) {
            first = false;
        } else {
            stream << ' ';
        }
        stream << i;
    }
    return stream;
}
} // namespace CppCp

#endif
