#ifndef CPPCP_FUNCTIONAL
#define CPPCP_FUNCTIONAL

#include <array>
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

} // namespace Functional

inline constexpr auto filter = Functional::FilterBuilder();

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
