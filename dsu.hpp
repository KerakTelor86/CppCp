#ifndef CPPCP_DSU
#define CPPCP_DSU

#include <numeric>
#include <vector>

#include "types.hpp"

namespace CppCp {

class DisjointSet {
public:
    DisjointSet(const usize size) : parent(size), counts(size, 1) {
        std::iota(std::begin(parent), std::end(parent), 0);
    }

    i32 get_root(const i32 x) const {
        if (parent[x] == x) {
            return x;
        }
        return parent[x] = get_root(parent[x]);
    }

    void make_root(const i32 x) {
        const i32 y = get_root(x);
        if (x == y) {
            return;
        }
        parent[x] = x;
        counts[x] = counts[y];
        parent[y] = x;
    }

    i32 get_count(const i32 x) const {
        return counts[x];
    }

    void join(const i32 x, const i32 y) {
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
