#ifndef CPPCP_HASH
#define CPPCP_HASH

#include <compare>
#include <concepts>
#include <functional>
#include <ostream>
#include <utility>

#include "debug.hpp"
#include "io.hpp"
#include "modint.hpp"
#include "random.hpp"

namespace CppCp {

inline constexpr i32 MUL_MIN = 1e3;
#if defined(ENABLE_HASH_DEBUG_MUL) && defined(LOCAL)
inline constexpr i32 MUL_MAX = 1e3;
#else
inline constexpr i32 MUL_MAX = 1e7;
#endif

template <typename ModInt, usize MaxLen>
auto precompute_mul_pows(const ModInt mul) {
    std::array<ModInt, MaxLen + 1> prec_pow;
    prec_pow[0] = 1;
    for (usize i = 1; i <= MaxLen; ++i) {
        prec_pow[i] = prec_pow[i - 1] * mul;
    }
    return prec_pow;
}

template <typename ModInt> using MulPowFunc = ModInt (*)(i64);

template <typename ModInt, MulPowFunc<ModInt>... MulPow> class RollingHash {
public:
    RollingHash() : len(0), hash{} {}
    RollingHash(const char c) : len(1) {
        std::fill(std::begin(hash), std::end(hash), c);
    }
    RollingHash(const string& s) : len(0), hash{} {
        for (const auto& i : s) {
            *this += i;
        }
    }

    RollingHash& operator+=(const RollingHash& other) {
        usize i = 0;
        ((hash[i] = hash[i] * MulPow(other.len) + other.hash[i], ++i), ...);
        len += other.len;
        return *this;
    }

    RollingHash& operator-=(const RollingHash& other) {
        debug_assert(
            len >= other.len, "resulting hash length cannot be negative"
        );
        usize i = 0;
        ((hash[i] -= other.hash[i] * MulPow(len - other.len), ++i), ...);
        len -= other.len;
        return *this;
    }

    RollingHash operator+(const RollingHash& other) const {
        RollingHash copy = *this;
        copy += other;
        return copy;
    }

    RollingHash operator-(const RollingHash& other) const {
        RollingHash copy = *this;
        copy -= other;
        return copy;
    }

    std::strong_ordering operator<=>(const RollingHash& other) const {
        if (len != other.len) {
            return len <=> other.len;
        }
        return hash <=> other.hash;
    }

    bool operator==(const RollingHash& other) const {
        return len == other.len && hash == other.hash;
    }

    friend std::ostream& operator<<(
        std::ostream& stream, const RollingHash& h
    ) {
        stream << '(' << h.len << " | " << h.hash << ')';
        return stream;
    }

private:
    i64 len;
    std::array<ModInt, sizeof...(MulPow)> hash;
};

#ifdef ENABLE_HASH_USING_MACRO

#define UsingHashHelper(ModInt, MaxN, Index)                            \
    const auto HASH_MUL##Index = rand_int(MUL_MIN, MUL_MAX);            \
    const auto HASH_MUL_POW##Index = precompute_mul_pows<ModInt, MaxN>( \
        HASH_MUL##Index                                                 \
    );                                                                  \
    ModInt hash_pow_mul##Index(const i64 exp) {                         \
        debug_assert(                                                   \
            exp < ssize(HASH_MUL_POW##Index), "tried to get exp > MaxN" \
        );                                                              \
        return HASH_MUL_POW##Index[exp];                                \
    }

#define UsingHashSingle(TargetTypeName, ModInt, MaxN) \
    UsingHashHelper(ModInt, MaxN, 0);                 \
    using TargetTypeName = RollingHash<ModInt, hash_pow_mul0>;

#if defined(ENABLE_HASH_DEBUG_MUL) && defined(LOCAL)

#define UsingHashDouble(...) UsingHashSingle(__VA_ARGS__)
#define UsingHashTriple(...) UsingHashSingle(__VA_ARGS__)
#define UsingHashQuadruple(...) UsingHashSingle(__VA_ARGS__)

#else

#define UsingHashDouble(TargetTypeName, ModInt, MaxN) \
    UsingHashHelper(ModInt, MaxN, 0);                 \
    UsingHashHelper(ModInt, MaxN, 1);                 \
    using TargetTypeName = RollingHash<ModInt, hash_pow_mul0, hash_pow_mul1>;

#define UsingHashTriple(TargetTypeName, ModInt, MaxN) \
    UsingHashHelper(ModInt, MaxN, 0);                 \
    UsingHashHelper(ModInt, MaxN, 1);                 \
    UsingHashHelper(ModInt, MaxN, 2);                 \
    using TargetTypeName = RollingHash<               \
        ModInt,                                       \
        hash_pow_mul0,                                \
        hash_pow_mul1,                                \
        hash_pow_mul2>;

#define UsingHashQuadruple(TargetTypeName, ModInt, MaxN) \
    UsingHashHelper(ModInt, MaxN, 0);                    \
    UsingHashHelper(ModInt, MaxN, 1);                    \
    UsingHashHelper(ModInt, MaxN, 2);                    \
    UsingHashHelper(ModInt, MaxN, 3);                    \
    using TargetTypeName = RollingHash<                  \
        ModInt,                                          \
        hash_pow_mul0,                                   \
        hash_pow_mul1,                                   \
        hash_pow_mul2,                                   \
        hash_pow_mul3>;

#endif

#endif

} // namespace CppCp

#endif
