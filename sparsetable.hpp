#ifndef CPPCP_SPARSETABLE
#define CPPCP_SPARSETABLE

#include <bit>
#include <vector>

#include "concepts.hpp"
#include "debug.hpp"
#include "types.hpp"

namespace CppCp {

template <typename Val, typename Op = std::plus<>>
    requires std::is_invocable_r_v<Val, Op, Val, Val>
             && std::assignable_from<Val&, Val>
class SparseTable {
public:
    template <typename T>
        requires IndexableContainer<T>
                     && std::assignable_from<Val&, decltype(T()[0])>
    SparseTable(const T& source, const Val& nil = Val())
        : len(std::size(source)),
          nil_value(nil),
          store(std::bit_width(len), std::vector<Val>(len, nil)) {
        std::copy(std::begin(source), std::end(source), std::begin(store[0]));
        for (usize i = 1; i <= std::size(store); ++i) {
            for (usize j = 0; j + (1 << i) <= len; ++j) {
                store[i][j] = op(
                    store[i - 1][j], store[i - 1][j + (1 << (i - 1))]
                );
            }
        }
    }

    Val query(const usize l, const usize r) const {
        debug_assert(l <= r, "l should be <= r");
        const auto lg = std::bit_width(r - l + 1) - 1;
        return op(store[lg][l], store[lg][r - (1 << lg) + 1]);
    }

    Val query_forward(usize idx, const usize step) const {
        debug_assert(
            idx + step <= len, "idx + step cannot be outside structure"
        );
        auto ans = nil_value;
        for (auto i = ssize(store) - 1; i >= 0; --i) {
            if (step & (1 << i)) {
                ans = op(ans, store[i][idx]);
                idx += 1 << i;
            }
        }
        return ans;
    }

    usize size() const {
        return len;
    }

private:
    const usize len;
    const Val nil_value;
    std::vector<std::vector<Val>> store;

    static constexpr auto op = Op();
};

} // namespace CppCp

#endif
