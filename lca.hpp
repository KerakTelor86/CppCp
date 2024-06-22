#ifndef CPPCP_LCA
#define CPPCP_LCA

#include <algorithm>
#include <vector>

#include "debug.hpp"
#include "sparsetable.hpp"
#include "types.hpp"

namespace CppCp {

class LCA {
public:
    LCA(const i32 num_verts,
        const std::vector<std::vector<i32>>& adj,
        const i32 root = 0)
        : n(num_verts),
          in(n),
          out(n),
          level(n),
          sparse(convert_to_data(adj, root)) {}

    i32 get_lca(const i32 u, const i32 v) const {
        return get_lca_distance(u, v).first;
    }

    i32 get_distance(const i32 u, const i32 v) const {
        return get_lca_distance(u, v).second;
    }

    std::pair<i32, i32> get_lca_distance(i32 u, i32 v) const {
        debug_assert(0 <= u && u < n, "u must be in [0, n)");
        debug_assert(0 <= v && v < n, "v must be in [0, n)");
        if (in[u] > out[v]) {
            std::swap(u, v);
        }
        const auto [lca_level, lca_index] = sparse.query(in[u], out[v]);
        return {lca_index, level[u] + level[v] - 2 * level[lca_index]};
    }

private:
    const i32 n;
    std::vector<i32> in, out, level;

    struct Min {
        std::pair<i32, i32> operator()(
            const std::pair<i32, i32>& a, const std::pair<i32, i32>& b
        ) const {
            return std::min(a, b);
        }
    };

    const SparseTable<std::pair<i32, i32>, Min> sparse;

    std::vector<std::pair<i32, i32>> convert_to_data(
        const std::vector<std::vector<i32>>& adj, const i32 root
    ) {
        std::vector<std::pair<i32, i32>> data;
        const auto dfs =
            [&](const auto& self, const i32 pos, const i32 last, const i32 depth
            ) -> void {
            debug_assert(0 <= pos && pos < n, "pos should be in [0, n)");

            in[pos] = size(data);
            out[pos] = size(data);
            level[pos] = depth;
            data.emplace_back(depth, pos);

            for (const auto& child : adj[pos]) {
                if (child == last) {
                    continue;
                }
                self(self, child, pos, depth + 1);

                out[pos] = size(data);
                data.emplace_back(depth, pos);
            }
        };
        dfs(dfs, root, -1, 0);
        return data;
    }
};

} // namespace CppCp

#endif
