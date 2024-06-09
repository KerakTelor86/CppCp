#ifndef CPPCP_SEGTREE
#define CPPCP_SEGTREE

#include <array>
#include <concepts>
#include <tuple>
#include <type_traits>
#include <vector>

#include "concepts.hpp"
#include "types.hpp"

namespace CppCp {

#ifndef CPPCP_SEGTREE_HELPER
#define CPPCP_SEGTREE_HELPER
namespace {

inline std::tuple<i32, i32, i32> compute_indices(
    i32 idx, i32 l, i32 r
) {
    i32 m = l + (r - l) / 2;
    i32 lc = idx + 1;
    i32 rc = idx + (m - l + 1) * 2;
    return {lc, rc, m};
}
} // namespace
#endif

template <typename Val, typename Op = std::plus<>>
    requires std::is_invocable_r_v<Val, Op, Val, Val>
             && std::assignable_from<Val&, Val>
class SegTree {
public:
    SegTree(const usize size, const Val& nil_value = Val())
        : store(2 * size, nil_value),
          nil(nil_value),
          len(size) {}

    template <typename T>
        requires IndexableContainer<T>
                     && std::assignable_from<Val&, decltype(T()[0])>
    SegTree(const T& source, const Val& nil_value = Val())
        : store(2 * std::size(source)),
          nil(nil_value),
          len(std::ssize(source)) {
        build(source, 0, 0, len - 1);
    }

    void set(const i32 pos, const Val& value) {
        mutate<true>(pos, value, 0, 0, len - 1);
    }

    void update(const i32 pos, const Val& value) {
        mutate<false>(pos, value, 0, 0, len - 1);
    }

    Val query(const i32 left, const i32 right) const {
        return query(left, right, 0, 0, len - 1);
    }

    usize size() const {
        return len;
    }

private:
    std::vector<Val> store;
    const Val nil;
    const i32 len;
    static constexpr auto op = Op();

    template <typename T>
        requires IndexableContainer<T>
                 && std::assignable_from<Val&, decltype(T()[0])>
    void build(
        const T& source, const i32 idx, const i32 l, const i32 r
    ) {
        if (l == r) {
            store[idx] = source[l];
            return;
        }
        auto [lc, rc, m] = compute_indices(idx, l, r);
        build(source, lc, l, m);
        build(source, rc, m + 1, r);
        store[idx] = op(store[lc], store[rc]);
    }

    template <bool replace>
    void mutate(
        const i32 u,
        const Val& w,
        const i32 idx,
        const i32 l,
        const i32 r
    ) {
        if (u > r || u < l) {
            return;
        }
        if (u == l && u == r) {
            if constexpr (replace) {
                store[idx] = w;
            } else {
                store[idx] = op(store[idx], w);
            }
            return;
        }
        auto [lc, rc, m] = compute_indices(idx, l, r);
        if (u <= m) {
            mutate<replace>(u, w, lc, l, m);
        } else {
            mutate<replace>(u, w, rc, m + 1, r);
        }
        store[idx] = op(store[lc], store[rc]);
    }

    Val query(
        const i32 u,
        const i32 v,
        const i32 idx,
        const i32 l,
        const i32 r
    ) const {
        if (u > r || v < l) {
            return nil;
        }
        if (u <= l && v >= r) {
            return store[idx];
        }
        auto [lc, rc, m] = compute_indices(idx, l, r);
        return op(query(u, v, lc, l, m), query(u, v, rc, m + 1, r));
    }
};

template <typename Val, usize Size, typename Op = std::plus<>>
    requires std::is_invocable_r_v<Val, Op, Val, Val>
             && std::assignable_from<Val&, Val>
class StaticSegTree {
public:
    StaticSegTree(const Val& nil_value = Val()) : nil(nil_value) {
        std::fill(std::begin(store), std::end(store), nil);
    }

    template <typename T>
        requires IndexableContainer<T>
                 && std::assignable_from<Val&, decltype(T()[0])>
    StaticSegTree(const T& source, const Val& nil_value = Val())
        : nil(nil_value) {
        build(source, 0, 0, Size - 1);
    }

    void set(const i32 pos, const Val& value) {
        mutate<true>(pos, value, 0, 0, Size - 1);
    }

    void update(const i32 pos, const Val& value) {
        mutate<false>(pos, value, 0, 0, Size - 1);
    }

    Val query(const i32 left, const i32 right) const {
        return query(left, right, 0, 0, Size - 1);
    }

    usize size() const {
        return Size;
    }

private:
    const Val nil;
    std::array<Val, 2 * Size> store;
    static constexpr auto op = Op();

    template <typename T>
        requires IndexableContainer<T>
                 && std::assignable_from<Val&, decltype(T()[0])>
    void build(
        const T& source, const i32 idx, const i32 l, const i32 r
    ) {
        if (l == r) {
            store[idx] = source[l];
            return;
        }
        auto [lc, rc, m] = compute_indices(idx, l, r);
        build(source, lc, l, m);
        build(source, rc, m + 1, r);
        store[idx] = op(store[lc], store[rc]);
    }

    template <bool replace>
    void mutate(
        const i32 u,
        const Val& w,
        const i32 idx,
        const i32 l,
        const i32 r
    ) {
        if (u > r || u < l) {
            return;
        }
        if (u == l && u == r) {
            if constexpr (replace) {
                store[idx] = w;
            } else {
                store[idx] = op(store[idx], w);
            }
            return;
        }
        auto [lc, rc, m] = compute_indices(idx, l, r);
        if (u <= m) {
            mutate<replace>(u, w, lc, l, m);
        } else {
            mutate<replace>(u, w, rc, m + 1, r);
        }
        store[idx] = op(store[lc], store[rc]);
    }

    Val query(
        const i32 u,
        const i32 v,
        const i32 idx,
        const i32 l,
        const i32 r
    ) const {
        if (u > r || v < l) {
            return nil;
        }
        if (u <= l && v >= r) {
            return store[idx];
        }
        auto [lc, rc, m] = compute_indices(idx, l, r);
        return op(query(u, v, lc, l, m), query(u, v, rc, m + 1, r));
    }
};

}; // namespace CppCp

#endif
