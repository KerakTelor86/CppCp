# CppCp

Header-only (read: copy-pasteable) C++20-ish competitive programming template. 

## Example usage

```cpp
// ...
#define ENABLE_HASH_USING_MACRO
// ...
#include "debug.hpp"
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
    const auto t = [s = s]() mutable {
        std::reverse(std::begin(s), std::end(s));
        return s;
    }();
    // ...
    const SegTree<Hash> hash_fwd(s), hash_rev(t);
    // ...
    write_line_debug(
        hash_fwd.query(0, std::ssize(s) - 1),
        hash_rev.query(0, std::ssize(t) - 1)
    );
    // ...
}
```

## Submitting to an online judge

Most online judges only accept single-file programs as submissions. You may use (and modify) the `scripts/copier.py` Python script to automatically replace include statements to this library.

Proper usage should look something like this: `cat code.cpp | python3 scripts/copier.py Documents/CP/Template/CppCp`
