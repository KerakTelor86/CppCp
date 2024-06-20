#ifndef CPPCP_LAZYSEGTREE
#define CPPCP_LAZYSEGTREE

#include <array>
#include <concepts>
#include <tuple>
#include <type_traits>
#include <vector>

#include "concepts.hpp"
#include "debug.hpp"
#include "types.hpp"

namespace CppCp {

#ifndef CPPCP_SEGTREE_HELPER
#define CPPCP_SEGTREE_HELPER
namespace {

inline std::tuple<i32, i32, i32> compute_indices(i32 idx, i32 l, i32 r) {
    i32 m = l + (r - l) / 2;
    i32 lc = idx + 1;
    i32 rc = idx + (m - l + 1) * 2;
    return {lc, rc, m};
}

} // namespace
#endif

template <
    typename Val,
    typename Lazy,
    typename Apply,
    typename ValOp = std::plus<>,
    typename LazyOp = std::plus<>>
    requires std::is_invocable_r_v<Val, ValOp, Val, Val>
             && std::assignable_from<Val&, Val>
             && std::is_invocable_r_v<Lazy, LazyOp, Lazy, Lazy>
             && std::assignable_from<Lazy&, Lazy>
             && std::equality_comparable<Lazy>
             && std::is_invocable_r_v<Val, Apply, Val, Lazy, i32, i32>
class LazySegTree {
public:
    LazySegTree(
        const usize size,
        const Val& nil_value = Val(),
        const Lazy& nil_lazy = Lazy()
    )
        : val_store(2 * size, nil_value),
          lazy_store(2 * size, nil_lazy),
          val_nil(nil_value),
          lazy_nil(nil_lazy),
          len(size) {}

    template <typename T>
        requires IndexableContainer<T>
                     && std::assignable_from<Val&, decltype(T()[0])>
    LazySegTree(
        const T& source,
        const Val& nil_value = Val(),
        const Lazy& nil_lazy = Lazy()
    )
        : val_store(2 * std::size(source), nil_value),
          lazy_store(2 * std::size(source), nil_lazy),
          val_nil(nil_value),
          lazy_nil(nil_lazy),
          len(std::ssize(source)) {
        build(source, 0, 0, len - 1);
    }

    void set(const i32 pos, const Val& value) {
        debug_assert(0 <= pos && pos < std::ssize(*this), "pos is invalid");
        set(pos, value, 0, 0, len - 1);
    }

    void update(const i32 left, const i32 right, const Lazy& lazy) {
        debug_assert(
            0 <= left && left < std::ssize(*this), "left pos is invalid"
        );
        debug_assert(
            0 <= right && right < std::ssize(*this), "right pos is invalid"
        );
        debug_assert(left <= right, "left pos is > right pos");
        update(left, right, lazy, 0, 0, len - 1);
    }

    Val query(const i32 left, const i32 right) const {
        debug_assert(
            0 <= left && left < std::ssize(*this), "left pos is invalid"
        );
        debug_assert(
            0 <= right && right < std::ssize(*this), "right pos is invalid"
        );
        debug_assert(left <= right, "left pos is > right pos");
        return query(left, right, 0, 0, len - 1);
    }

    usize size() const {
        return len;
    }

private:
    mutable std::vector<Val> val_store;
    mutable std::vector<Lazy> lazy_store;
    const Val val_nil;
    const Lazy lazy_nil;
    const i32 len;
    static constexpr auto val_op = ValOp();
    static constexpr auto lazy_op = LazyOp();
    static constexpr auto apply = Apply();

    template <typename T>
        requires IndexableContainer<T>
                 && std::assignable_from<Val&, decltype(T()[0])>
    void build(const T& source, const i32 idx, const i32 l, const i32 r) {
        if (l == r) {
            val_store[idx] = source[l];
            return;
        }
        auto [lc, rc, m] = compute_indices(idx, l, r);
        build(source, lc, l, m);
        build(source, rc, m + 1, r);
        val_store[idx] = val_op(val_store[lc], val_store[rc]);
    }

    void propagate(i32 idx, i32 l, i32 r) const {
        if (lazy_store[idx] == lazy_nil) {
            return;
        }
        if (l != r) {
            const auto [lc, rc, m] = compute_indices(idx, l, r);
            lazy_store[lc] = lazy_op(lazy_store[lc], lazy_store[idx]);
            lazy_store[rc] = lazy_op(lazy_store[rc], lazy_store[idx]);
        }
        val_store[idx] = apply(val_store[idx], lazy_store[idx], l, r);
        lazy_store[idx] = lazy_nil;
    }

    void set(
        const i32 u, const Val& w, const i32 idx, const i32 l, const i32 r
    ) {
        propagate(idx, l, r);
        if (u > r || u < l) {
            return;
        }
        if (u == l && u == r) {
            val_store[idx] = w;
            return;
        }
        auto [lc, rc, m] = compute_indices(idx, l, r);
        set(u, w, lc, l, m);
        set(u, w, rc, m + 1, r);
        val_store[idx] = val_op(val_store[lc], val_store[rc]);
    }

    void update(
        const i32 u,
        const i32 v,
        const Lazy& w,
        const i32 idx,
        const i32 l,
        const i32 r
    ) {
        propagate(idx, l, r);
        if (u > r || v < l) {
            return;
        }
        if (u <= l && v >= r) {
            lazy_store[idx] = w;
            propagate(idx, l, r);
            return;
        }
        auto [lc, rc, m] = compute_indices(idx, l, r);
        update(u, v, w, lc, l, m);
        update(u, v, w, rc, m + 1, r);
        val_store[idx] = val_op(val_store[lc], val_store[rc]);
    }

    Val query(const i32 u, const i32 v, const i32 idx, const i32 l, const i32 r)
        const {
        propagate(idx, l, r);
        if (u > r || v < l) {
            return val_nil;
        }
        if (u <= l && v >= r) {
            return val_store[idx];
        }
        auto [lc, rc, m] = compute_indices(idx, l, r);
        return val_op(query(u, v, lc, l, m), query(u, v, rc, m + 1, r));
    }
};

template <
    typename Val,
    typename Lazy,
    usize Size,
    typename Apply,
    typename ValOp = std::plus<>,
    typename LazyOp = std::plus<>>
    requires std::is_invocable_r_v<Val, ValOp, Val, Val>
             && std::assignable_from<Val&, Val>
             && std::is_invocable_r_v<Lazy, LazyOp, Lazy, Lazy>
             && std::assignable_from<Lazy&, Lazy>
             && std::equality_comparable<Lazy>
             && std::is_invocable_r_v<Val, Apply, Val, Lazy, i32, i32>
class StaticLazySegTree {
public:
    StaticLazySegTree(
        const Val& nil_value = Val(), const Lazy& nil_lazy = Lazy()
    )
        : val_nil(nil_value),
          lazy_nil(nil_lazy) {
        std::fill(std::begin(val_store), std::end(val_store), val_nil);
        std::fill(std::begin(lazy_store), std::end(lazy_store), lazy_nil);
    }

    template <typename T>
        requires IndexableContainer<T>
                     && std::assignable_from<Val&, decltype(T()[0])>
    StaticLazySegTree(
        const T& source,
        const Val& nil_value = Val(),
        const Lazy& nil_lazy = Lazy()
    )
        : val_nil(nil_value),
          lazy_nil(nil_lazy) {
        build(source, 0, 0, Size - 1);
        std::fill(std::begin(lazy_store), std::end(lazy_store), lazy_nil);
    }

    void set(const i32 pos, const Val& value) {
        debug_assert(0 <= pos && pos < std::ssize(*this), "pos is invalid");
        set(pos, value, 0, 0, Size - 1);
    }

    void update(const i32 left, const i32 right, const Lazy& lazy) {
        debug_assert(
            0 <= left && left < std::ssize(*this), "left pos is invalid"
        );
        debug_assert(
            0 <= right && right < std::ssize(*this), "right pos is invalid"
        );
        debug_assert(left <= right, "left pos is > right pos");
        update(left, right, lazy, 0, 0, Size - 1);
    }

    Val query(const i32 left, const i32 right) const {
        debug_assert(
            0 <= left && left < std::ssize(*this), "left pos is invalid"
        );
        debug_assert(
            0 <= right && right < std::ssize(*this), "right pos is invalid"
        );
        debug_assert(left <= right, "left pos is > right pos");
        return query(left, right, 0, 0, Size - 1);
    }

    usize size() const {
        return Size;
    }

private:
    mutable std::array<Val, 2 * Size> val_store;
    mutable std::array<Lazy, 2 * Size> lazy_store;
    const Val val_nil;
    const Lazy lazy_nil;
    static constexpr auto val_op = ValOp();
    static constexpr auto lazy_op = LazyOp();
    static constexpr auto apply = Apply();

    template <typename T>
        requires IndexableContainer<T>
                 && std::assignable_from<Val&, decltype(T()[0])>
    void build(const T& source, const i32 idx, const i32 l, const i32 r) {
        if (l == r) {
            val_store[idx] = source[l];
            return;
        }
        auto [lc, rc, m] = compute_indices(idx, l, r);
        build(source, lc, l, m);
        build(source, rc, m + 1, r);
        val_store[idx] = val_op(val_store[lc], val_store[rc]);
    }

    void propagate(i32 idx, i32 l, i32 r) const {
        if (lazy_store[idx] == lazy_nil) {
            return;
        }
        if (l != r) {
            const auto [lc, rc, m] = compute_indices(idx, l, r);
            lazy_store[lc] = lazy_op(lazy_store[lc], lazy_store[idx]);
            lazy_store[rc] = lazy_op(lazy_store[rc], lazy_store[idx]);
        }
        val_store[idx] = apply(val_store[idx], lazy_store[idx], l, r);
        lazy_store[idx] = lazy_nil;
    }

    void set(
        const i32 u, const Val& w, const i32 idx, const i32 l, const i32 r
    ) {
        propagate(idx, l, r);
        if (u > r || u < l) {
            return;
        }
        if (u == l && u == r) {
            val_store[idx] = w;
            return;
        }
        auto [lc, rc, m] = compute_indices(idx, l, r);
        set(u, w, lc, l, m);
        set(u, w, rc, m + 1, r);
        val_store[idx] = val_op(val_store[lc], val_store[rc]);
    }

    void update(
        const i32 u,
        const i32 v,
        const Lazy& w,
        const i32 idx,
        const i32 l,
        const i32 r
    ) {
        propagate(idx, l, r);
        if (u > r || v < l) {
            return;
        }
        if (u <= l && v >= r) {
            lazy_store[idx] = w;
            propagate(idx, l, r);
            return;
        }
        auto [lc, rc, m] = compute_indices(idx, l, r);
        update(u, v, w, lc, l, m);
        update(u, v, w, rc, m + 1, r);
        val_store[idx] = val_op(val_store[lc], val_store[rc]);
    }

    Val query(const i32 u, const i32 v, const i32 idx, const i32 l, const i32 r)
        const {
        propagate(idx, l, r);
        if (u > r || v < l) {
            return val_nil;
        }
        if (u <= l && v >= r) {
            return val_store[idx];
        }
        auto [lc, rc, m] = compute_indices(idx, l, r);
        return val_op(query(u, v, lc, l, m), query(u, v, rc, m + 1, r));
    }
};

} // namespace CppCp

#endif
