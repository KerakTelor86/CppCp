#ifndef CPPCP_DSU
#define CPPCP_DSU

#include <numeric>
#include <vector>

#include "debug.hpp"
#include "types.hpp"

namespace CppCp {

class DisjointSet {
public:
    DisjointSet(const usize size) : parent(size), counts(size, 1) {
        std::iota(std::begin(parent), std::end(parent), 0);
    }

    i32 get_root(const i32 x) const {
        debug_assert(
            0 <= x && x < std::ssize(*this), "trying to get root of invalid x"
        );
        if (parent[x] == x) {
            return x;
        }
        return parent[x] = get_root(parent[x]);
    }

    void make_root(const i32 x) {
        debug_assert(
            0 <= x && x < std::ssize(*this), "trying to make root invalid x"
        );
        const i32 y = get_root(x);
        if (x == y) {
            return;
        }
        parent[x] = x;
        counts[x] = counts[y];
        parent[y] = x;
    }

    i32 get_count(const i32 x) const {
        debug_assert(
            0 <= x && x < std::ssize(*this), "trying to get count of invalid x"
        );
        return counts[x];
    }

    void join(const i32 x, const i32 y) {
        debug_assert(
            0 <= x && x < std::ssize(*this), "trying to join invalid x"
        );
        debug_assert(
            0 <= y && y < std::ssize(*this), "trying to join invalid y"
        );
        i32 a = get_root(x), b = get_root(y);
        if (a == b) {
            return;
        }
        if (counts[a] > counts[b]) {
            std::swap(a, b);
        }
        counts[a] += counts[b];
        parent[b] = a;
    }

    usize size() const {
        return std::size(parent);
    }

private:
    mutable std::vector<i32> parent, counts;
};

} // namespace CppCp

#endif
