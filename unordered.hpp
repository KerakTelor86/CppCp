#ifndef CPPCP_SAFEMAP
#define CPPCP_SAFEMAP

#include <chrono>
#include <unordered_map>
#include <unordered_set>

#include "types.hpp"

namespace CppCp {

struct UnorderedHash {
    static u64 splitmix64(u64 x) {
        x += 0x9e3779b97f4a7c15;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        return x ^ (x >> 31);
    }

    usize operator()(const u64 x) const {
        static const u64 FIXED_RANDOM = std::chrono::steady_clock::now()
                                            .time_since_epoch()
                                            .count();
        return splitmix64(x + FIXED_RANDOM);
    }
};

#if __cplusplus >= 202002L

template <typename K>
concept UnorderedHashable = requires(
    const UnorderedHash& hasher, const K& value
) {
    { hasher(value) } -> std::same_as<usize>;
};

template <UnorderedHashable K, typename V>
using UnorderedMap = std::unordered_map<K, V, UnorderedHash>;

template <UnorderedHashable K>
using UnorderedSet = std::unordered_set<K, UnorderedHash>;

#else

template <typename K, typename V>
using UnorderedMap = std::unordered_map<K, V, UnorderedHash>;

template <typename K> using UnorderedSet = std::unordered_set<K, UnorderedHash>;
#endif

}; // namespace CppCp

#endif
