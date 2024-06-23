# CppCp

Header-only (read: copy-pasteable) C++20-ish competitive programming library. 

## Example usage

```cpp
// ...
#define ENABLE_HASH_USING_MACRO
// ...
#include "debug.hpp"
#include "fluent.hpp"
#include "hash.hpp"
#include "io.hpp"
#include "modint.hpp"
#include "segtree.hpp"
#include "types.hpp"
// ...
using namespace CppCp;
// ...
constexpr i32 MAX_N = 1e5;
UsingHashDouble(Hash, ModIntMersenne, MAX_N);
// ...
int main() {
    // ...
    const auto s = read<std::string>();
    const auto n = ssize(s);
    // ...
    const auto t = fluent(s).reversed().get();
    const SegTree<Hash> hash_fwd(s), hash_rev(t);
    write_line_debug(
        hash_fwd.query(0, std::ssize(s) - 1),
        hash_rev.query(0, std::ssize(t) - 1)
    );
    // ...
    const auto q = read<i32>();
    const auto ans = fluent(read<i32, i32>(q))
                         .map([&](const std::tuple<i32, i32>& query) {
                             const auto [l, r] = query;
                             return hash_fwd.query(l, r)
                                    == hash_rev.query(n - r - 1, n - l - 1);
                         })
                         .get();
    for (const auto& cur : ans) {
        if (cur) {
            write_line("Same");
        } else {
            write_line("Not same");
        }
    }
    // ...
}
```

## Submitting to an online judge

Most online judges only accept single-file programs as submissions. You may use (and modify) the `scripts/copier.py` Python script to automatically replace include statements to this library.

Proper usage should look something like this: `cat code.cpp | python3 scripts/copier.py Documents/CP/Template/CppCp`
