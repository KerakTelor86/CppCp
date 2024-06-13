#ifndef CPPCP_ZIP
#define CPPCP_ZIP

#include <utility>
#include <vector>

#include "types.hpp"

namespace CppCp {

namespace {

template <typename T, usize... Idx>
void reserve_helper(T& vec_tuple, usize size, std::index_sequence<Idx...>) {
    (std::get<Idx>(vec_tuple).reserve(size), ...);
}

template <typename T, typename U, usize... Idx>
void unzip_helper(T& vec_tuple, const U& row, std::index_sequence<Idx...>) {
    ((std::get<Idx>(vec_tuple).push_back(std::get<Idx>(row))), ...);
}

} // namespace

template <typename... T> auto unzip(const std::vector<std::tuple<T...>>& vec) {
    std::tuple<std::vector<T>...> ret;

    reserve_helper(ret, size(vec), std::index_sequence_for<T...>{});
    for (const auto& row : vec) {
        unzip_helper(ret, row, std::index_sequence_for<T...>{});
    }

    return ret;
}

} // namespace CppCp

#endif
