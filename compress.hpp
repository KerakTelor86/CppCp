#ifndef CPPCP_COMPRESS
#define CPPCP_COMPRESS

#include <algorithm>
#if __cplusplus >= 202002L
#include <concepts>
#endif
#include <cstdarg>
#include <initializer_list>
#include <iterator>
#include <map>
#include <vector>

#include "types.hpp"
#include "unordered.hpp"

namespace CppCp {

template <typename T, typename Store> class LiveCompressor {
public:
    void insert(const T& element) {
        if (store.find(element) == std::end(store)) {
            store.emplace(element, ptr++);
        }
    }

    void insert(const std::vector<T>& elements) {
        for (const auto& i : elements) {
            insert(i);
        }
    }

    usize compress(const T& val) const {
        return store.find(val)->second;
    }

    std::vector<usize> compress(const std::vector<T>& vals) const {
        std::vector<usize> ret;
        ret.reserve(std::size(vals));
        std::transform(
            std::begin(vals),
            std::end(vals),
            std::back_inserter(ret),
            [&](const T& x) { return compress(x); }
        );
        return ret;
    }

    usize size() const {
        return std::size(store);
    }

private:
    usize ptr = 0;

    Store store;
};

#if __cplusplus >= 202002L
template <std::totally_ordered T>
#else
template <typename T>
#endif
using MapCompressor = LiveCompressor<T, std::map<T, usize>>;

#if __cplusplus >= 202002L
template <UnorderedHashable T>
#else
template <typename T>
#endif
using UnorderedMapCompressor = LiveCompressor<T, UnorderedMap<T, usize>>;

template <typename T> class FinalizedCompressor;

#if __cplusplus >= 202002L
template <std::totally_ordered T>
#else
template <typename T>
#endif
class DeferredCompressor {
public:
    DeferredCompressor() {}
    DeferredCompressor(std::initializer_list<std::vector<T>> init) {
        usize total_size = 0;
        for (const auto& i : init) {
            total_size += std::size(i);
        }
        store.reserve(total_size);

        for (const auto& i : init) {
            store.insert(std::end(store), std::begin(i), std::end(i));
        }
    }

    void insert(const T& element) {
        store.push_back(element);
    }

    void insert(const std::vector<T>& elements) {
        for (const auto& i : elements) {
            insert(i);
        }
    }

    FinalizedCompressor<T> finalize() const {
        auto sorted_unique = store;
        std::sort(std::begin(sorted_unique), std::end(sorted_unique));
        sorted_unique.erase(
            std::unique(std::begin(sorted_unique), std::end(sorted_unique)),
            std::end(sorted_unique)
        );
        return FinalizedCompressor(std::move(sorted_unique));
    }

private:
    std::vector<T> store;
};

template <typename T> class FinalizedCompressor {
    friend DeferredCompressor<T>;

public:
    usize compress(const T& val) const {
        const auto pos = std::lower_bound(
            std::begin(store), std::end(store), val
        );
        return pos - std::begin(store);
    }

    std::vector<usize> compress(const std::vector<T>& vals) const {
        std::vector<usize> ret;
        ret.reserve(std::size(vals));
        std::transform(
            std::begin(vals),
            std::end(vals),
            std::back_inserter(ret),
            [&](const T& x) { return compress(x); }
        );
        return ret;
    }

    usize size() const {
        return std::size(store);
    }

private:
    FinalizedCompressor(std::vector<T>&& data) : store(data) {}

    const std::vector<T> store;
};

} // namespace CppCp

#endif
