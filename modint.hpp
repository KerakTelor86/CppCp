#ifndef CPPCP_MODINT
#define CPPCP_MODINT

#include <iostream>
#include <string>

#include "types.hpp"
#include "unordered.hpp"

namespace CppCp {
namespace {

template <typename T, typename U, T MOD> class ModInt {
public:
#if __cplusplus >= 202002L
    constexpr ModInt() : rep(0) {}
    constexpr ModInt(const T value) : rep(value) {}
#else
    ModInt() : rep(0) {}
    ModInt(const T value) : rep(value) {}
#endif

    ModInt& operator+=(const ModInt& other) {
        rep = (rep + other.rep) % MOD;
        return *this;
    }
    ModInt& operator*=(const ModInt& other) {
        rep = ((U)rep * other.rep) % MOD;
        return *this;
    }
    ModInt& operator-=(const ModInt& other) {
        rep = ((rep - other.rep) % MOD + MOD) % MOD;
        return *this;
    }
    ModInt& operator/=(const ModInt& other) {
        return *this *= other.inv();
    }

    ModInt operator+(const ModInt& other) const {
        ModInt copy = *this;
        copy += other;
        return copy;
    }
    ModInt operator-(const ModInt& other) const {
        ModInt copy = *this;
        copy -= other;
        return copy;
    }
    ModInt operator*(const ModInt& other) const {
        ModInt copy = *this;
        copy *= other;
        return copy;
    }
    ModInt operator/(const ModInt& other) const {
        ModInt copy = *this;
        copy /= other;
        return copy;
    }

    ModInt pow(const i64 exp) const {
        if (exp == 0) {
            return 1;
        }
        if (exp % 2 == 1) {
            return *this * pow(exp - 1);
        }
        const auto temp = pow(exp / 2);
        return temp * temp;
    }

    ModInt inv() const {
#ifdef ENABLE_MODINT_INV_CACHE
        const auto it = inv_cache.find(rep);
        if (it != std::end(inv_cache)) {
            return ModInt(it->second);
        }
        const auto res = pow(MOD - 2);
        inv_cache[rep] = res.rep;
        return res;
#else
        return pow(MOD - 2);
#endif
    }

#if __cplusplus >= 202002L
    std::strong_ordering operator<=>(const ModInt& other) const {
        return rep <=> other.rep;
    }
#else
    bool operator<(const ModInt& other) const {
        return rep < other.rep;
    }

    bool operator>(const ModInt& other) const {
        return rep > other.rep;
    }
#endif

    bool operator==(const ModInt& other) const {
        return rep == other.rep;
    }

    friend std::istream& operator>>(std::istream& stream, ModInt& value) {
        stream >> value.rep;
        return stream;
    }

    friend std::ostream& operator<<(std::ostream& stream, const ModInt& value) {
        stream << value.rep;
        return stream;
    }

private:
    T rep;

#ifdef ENABLE_MODINT_INV_CACHE
    static UnorderedMap<T, T> inv_cache;
#endif
};

#ifdef ENABLE_MODINT_INV_CACHE
template <typename T, typename U, T MOD>
UnorderedMap<T, T> ModInt<T, U, MOD>::inv_cache = UnorderedMap<T, T>();
#endif
} // namespace

template <i32 MOD> using ModInt32 = ModInt<i32, i64, MOD>;
template <i64 MOD> using ModInt64 = ModInt<i64, i128, MOD>;

using ModInt998244353 = ModInt32<998244353>;
using ModInt1000000007 = ModInt32<1000000007>;
using ModIntMersenne = ModInt64<(1LL << 61) - 1>;

}; // namespace CppCp

#endif
