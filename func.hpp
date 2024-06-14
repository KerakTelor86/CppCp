#ifndef CPPCP_FUNCTIONAL
#define CPPCP_FUNCTIONAL

#include <array>
#include <numeric>
#include <vector>

#include "concepts.hpp"
#include "types.hpp"

namespace CppCp {

namespace Functional {

template <typename Func> struct Filter {
    Filter(const Func& _func) : func(_func) {}
    const Func func;
};

struct FilterBuilder {
    auto operator()(const auto& func) const {
        return Filter(func);
    }
};

template <typename Init, typename Func> struct Fold {
    Fold(const Init& _init, const Func& _func) : init(_init), func(_func) {}
    const Init init;
    const Func func;
};

struct FoldBuilder {
    auto operator()(const auto& initial, const auto& func) const {
        return Fold(initial, func);
    }
};

template <typename Init, typename Func> struct RunningFold {
    RunningFold(const Init& _init, const Func& _func)
        : init(_init),
          func(_func) {}
    const Init init;
    const Func func;
};

struct RunningFoldBuilder {
    auto operator()(const auto& initial, const auto& func) const {
        return RunningFold(initial, func);
    }
};

template <typename Func> struct Reduce {
    Reduce(const Func& _func) : func(_func) {}
    const Func func;
};

struct ReduceBuilder {
    auto operator()(const auto& func) const {
        return Reduce(func);
    }
};

template <typename T, typename Func>
    requires Lambda<Func, T>
auto operator|(const std::vector<T>& vec, const Func& func) {
    using Ret = decltype(func(T()));
    std::vector<Ret> ret;
    ret.reserve(std::ssize(vec));
    for (const auto& i : vec) {
        ret.push_back(func(i));
    }
    return ret;
}

template <typename T, typename Func>
auto operator|(const std::vector<T>& vec, const Filter<Func>& filter) {
    std::vector<T> ret;
    ret.reserve(std::ssize(vec));
    for (const auto& i : vec) {
        if (filter.func(i)) {
            ret.push_back(i);
        }
    }
    return ret;
}

template <typename T, typename Init, typename Func>
auto operator|(const std::vector<T>& vec, const Fold<Init, Func>& fold) {
    auto ret = fold.init;
    for (const auto& i : vec) {
        ret = fold.func(ret, i);
    }
    return ret;
}

template <typename T, typename Init, typename Func>
auto operator|(
    const std::vector<T>& vec, const RunningFold<Init, Func>& running_fold
) {
    using Ret = decltype(running_fold.func(Init(), T()));
    std::vector<Ret> ret{running_fold.init};
    for (const auto& i : vec) {
        ret.push_back(running_fold.func(ret.back(), i));
    }
    return ret;
}

template <typename T, typename Func>
auto operator|(const std::vector<T>& vec, const Reduce<Func>& reduce) {
    auto ret = vec[0];
    for (usize i = 1; i < size(vec); ++i) {
        ret = reduce.func(ret, vec[i]);
    }
    return ret;
}

} // namespace Functional

inline constexpr auto filter = Functional::FilterBuilder();
inline constexpr auto fold = Functional::FoldBuilder();
inline constexpr auto running_fold = Functional::RunningFoldBuilder();
inline constexpr auto reduce = Functional::ReduceBuilder();

template <typename T> std::vector<T> int_range(const T lo, const T hi) {
    std::vector<T> ret(hi - lo + 1);
    std::iota(std::begin(ret), std::end(ret), lo);
    return ret;
}

template <typename T>
auto pipe_vec(const std::vector<T>& vec, const auto&... funcs) {
    using Functional::operator|;
    return (vec | ... | funcs);
}

template <typename T, usize Len>
auto pipe_vec(const std::array<T, Len>& arr, const auto&... funcs) {
    return pipe_vec(std::vector(std::begin(arr), std::end(arr)), funcs...);
}

} // namespace CppCp

#endif
