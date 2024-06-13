#ifndef CPPCP_MATH
#define CPPCP_MATH

#include <algorithm>
#include <array>
#include <climits>
#include <numeric>
#include <vector>

#include "random.hpp"
#include "types.hpp"

namespace CppCp {

inline i64 mod_pow(const i64 base, const i64 exp, const i64 mod) {
    if (exp == 0) {
        return 1 % mod;
    }
    if (exp % 2 == 1) {
        return ((i128)base * mod_pow(base, exp - 1, mod)) % mod;
    }
    const i64 temp = mod_pow(base, exp / 2, mod);
    return ((i128)temp * temp) % mod;
}

namespace MillerRabin {
constexpr std::array PRIMES{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};

inline bool miller_rabin(const i64 n, const i64 a, const i64 d, const i32 s) {
    i64 x = mod_pow(a, d, n);
    if (x == 1 || x == n - 1) {
        return 0;
    }
    for (i32 i = 0; i < s; ++i) {
        x = ((i128)x * x) % n;
        if (x == n - 1) {
            return 0;
        }
    }
    return 1;
}

} // namespace MillerRabin

inline bool is_prime(const i64 x) {
    if (x < 2) {
        return false;
    }
    i32 r = 0;
    i64 d = x - 1;
    while (d % 2 == 0) {
        d /= 2;
        ++r;
    }
    for (const auto& i : MillerRabin::PRIMES) {
        if (x == i) {
            return true;
        }
        if (MillerRabin::miller_rabin(x, i, d, r)) {
            return false;
        }
    }
    return true;
}

namespace PollardRho {
inline i64 f(const i64 x, const i64 b, const i64 n) {
    return (((i128)x * x) % n + b) % n;
}

inline i64 rho(const i64 n) {
    if (n % 2 == 0) {
        return 2;
    }
    const i64 b = rand_int(0LL, LLONG_MAX);
    i64 x = rand_int(0LL, LLONG_MAX);
    i64 y = x;
    while (true) {
        x = f(x, b, n);
        y = f(f(y, b, n), b, n);
        const i64 d = std::gcd(std::abs(x - y), n);
        if (d != 1) {
            return d;
        }
    }
}

inline void pollard_rho(const i64 n, std::vector<i64>& res) {
    if (n == 1) {
        return;
    }
    if (is_prime(n)) {
        res.push_back(n);
        return;
    }
    const i64 d = rho(n);
    pollard_rho(d, res);
    pollard_rho(n / d, res);
}

} // namespace PollardRho

template <bool sorted = true> inline std::vector<i64> factorize(const i64 n) {
    std::vector<i64> res;
    PollardRho::pollard_rho(n, res);
    if constexpr (sorted) {
        std::sort(std::begin(res), std::end(res));
    }
    return res;
}

template <typename ModInt> class Combinatorics {
public:
    Combinatorics(const i32 max_n) : fact(max_n + 1), inv_fact(max_n + 1) {

        fact[0] = 1;
        for (i32 i = 1; i <= max_n; ++i) {
            fact[i] = fact[i - 1] * i;
        }

        inv_fact[max_n] = fact[max_n].inv();
        for (int i = max_n - 1; i >= 0; --i) {
            inv_fact[i] = inv_fact[i + 1] * (i + 1);
        }
    }

    ModInt factorial(const i32 n) const {
        return fact[n];
    }

    ModInt perm(const i32 n, const i32 k) const {
        if (k > n) {
            return 0;
        }
        return fact[n] * inv_fact[n - k];
    }

    ModInt comb(const i32 n, const i32 k) const {
        if (k > n) {
            return 0;
        }
        return fact[n] * inv_fact[n - k] * inv_fact[k];
    }

private:
    std::vector<ModInt> fact, inv_fact;
};

} // namespace CppCp

#endif
