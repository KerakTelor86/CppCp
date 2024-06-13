#ifndef CPPCP_RANDOM
#define CPPCP_RANDOM

#include <algorithm>
#include <chrono>
#include <random>

#include "types.hpp"

namespace CppCp {

static std::mt19937_64 rng64(
    std::chrono::steady_clock::now().time_since_epoch().count()
);

static std::mt19937 rng32(
    std::chrono::steady_clock::now().time_since_epoch().count()
);

template <typename T> T rand_int(const T lo, const T hi) {
    if constexpr (std::same_as<T, u64> || std::same_as<T, i64>) {
        return std::uniform_int_distribution<T>(lo, hi)(rng64);
    } else {
        return std::uniform_int_distribution<T>(lo, hi)(rng32);
    }
}

template <typename T> T rand_range(const T size) {
    return rand_int<T>(0, size - 1);
}

template <typename T> T rand_choice(const std::vector<T>& elements) {
    return elements[rand_range(std::size(elements))];
}

template <typename T, usize Size>
T rand_choice(const std::array<T, Size>& elements) {
    return elements[rand_range(std::size(elements))];
}

template <typename T>
std::vector<T> rand_choices(const std::vector<T>& elements, const usize count) {
    std::vector<T> res(std::begin(elements), std::end(elements));
    std::shuffle(std::begin(res), std::end(res), rng32);
    res.resize(count);
    return res;
}

template <typename T, usize Size>
std::vector<T> rand_choices(
    const std::array<T, Size>& elements, const usize count
) {
    std::vector<T> res(std::begin(elements), std::end(elements));
    std::shuffle(std::begin(res), std::end(res), rng32);
    res.resize(count);
    return res;
}

} // namespace CppCp

#endif
