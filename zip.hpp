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

template <typename T, typename U, usize... Idx>
void zip_helper(const T& vec_tuple, U& row, usize row_idx, std::index_sequence<Idx...>) {
    debug_assert(
        ((row_idx < std::size(std::get<Idx>(vec_tuple))) && ...),
        "size not enough to zip (less than size of first vector in the tuple)"
    );
    ((std::get<Idx>(row) = std::get<Idx>(vec_tuple)[row_idx]), ...);
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

template <typename T, typename... Rest>
auto zip(const std::tuple<std::vector<T>, std::vector<Rest>...>& tup) {
    const auto size = std::size(std::get<0>(tup));
    using Row = std::tuple<T, Rest...>;

    std::vector<Row> ret;
    ret.reserve(size);

    for (usize i = 0; i < size; ++i) {
        auto row = Row();
        zip_helper(tup, row, i, std::index_sequence_for<T, Rest...>{});
        ret.push_back(std::move(row));
    }

    return ret;
}

template <typename... T> auto zip(const std::vector<T>... vecs) {
    return zip(make_tuple(vecs...));
}

} // namespace CppCp

#endif
