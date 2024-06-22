#ifndef CPPCP_GRAPH
#define CPPCP_GRAPH

#include <vector>

#include "types.hpp"

namespace CppCp {

inline std::vector<std::vector<i32>> to_adj_list(
    const i32 num_verts,
    const std::vector<std::tuple<i32, i32>>& edges,
    const bool directed = false
) {
    std::vector<std::vector<i32>> ret(num_verts);
    for (const auto& [u, v] : edges) {
        ret[u].push_back(v);
        if (!directed) {
            ret[v].push_back(u);
        }
    }
    return ret;
}

template <typename T>
inline std::vector<std::vector<i32>> to_adj_list(
    const i32 num_verts,
    const std::vector<std::tuple<i32, i32, T>>& edges,
    const bool directed = false
) {
    std::vector<std::vector<std::pair<i32, T>>> ret(num_verts);
    for (const auto& [u, v, w] : edges) {
        ret[u].emplace_back(v, w);
        if (!directed) {
            ret[v].emplace_back(u, w);
        }
    }
    return ret;
}

} // namespace CppCp

#endif
