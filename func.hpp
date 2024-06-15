#ifndef CPPCP_FUNCTIONAL
#define CPPCP_FUNCTIONAL

#include <algorithm>
#include <array>
#include <map>
#include <numeric>
#include <vector>

#include "concepts.hpp"
#include "types.hpp"
#include "unordered.hpp"
#include "zip.hpp"

namespace CppCp {

template <typename R = usize, typename T>
std::vector<R> index_range_of(const std::vector<T>& vec);

namespace Functional {

template <typename Func> struct Filter {
    constexpr Filter(const Func& _func) : func(_func) {}
    const Func func;
};

template <typename Func> struct Partition {
    constexpr Partition(const Func& _func) : func(_func) {}
    const Func func;
};

template <typename Func> struct GroupBy {
    constexpr GroupBy(const Func& _func) : func(_func) {}
    const Func func;
};

template <typename Init, typename Func> struct Fold {
    constexpr Fold(const Init& _init, const Func& _func)
        : init(_init),
          func(_func) {}
    const Init init;
    const Func func;
};

template <typename Init, typename Func> struct RunningFold {
    constexpr RunningFold(const Init& _init, const Func& _func)
        : init(_init),
          func(_func) {}
    const Init init;
    const Func func;
};

template <typename Func> struct Reduce {
    constexpr Reduce(const Func& _func) : func(_func) {}
    const Func func;
};

template <typename Cmp> struct Sorted {
    constexpr Sorted() {}
};

struct Unique {};

struct WithIndex {};

struct FilterBuilder {
    constexpr auto operator()(const auto& func) const {
        return Filter(func);
    }
};

struct PartitionBuilder {
    constexpr auto operator()(const auto& func) const {
        return Partition(func);
    }
};

struct GroupByBuilder {
    constexpr auto operator()(const auto& func) const {
        return GroupBy(func);
    }
};

struct FoldBuilder {
    constexpr auto operator()(const auto& initial, const auto& func) const {
        return Fold(initial, func);
    }
};

struct RunningFoldBuilder {
    constexpr auto operator()(const auto& initial, const auto& func) const {
        return RunningFold(initial, func);
    }
};

struct ReduceBuilder {
    constexpr auto operator()(const auto& func) const {
        return Reduce(func);
    }
};

struct SortedBuilder {
    constexpr auto operator()() const {
        return Sorted<std::less<>>();
    }

    template <typename Cmp>
    constexpr auto operator()([[maybe_unused]] const Cmp& compare) const {
        return Sorted<Cmp>();
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
    requires LambdaWithRet<bool, Func, T>
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

template <typename T, typename Func>
    requires LambdaWithRet<bool, Func, T>
auto operator|(const std::vector<T>& vec, const Partition<Func>& partition) {
    std::pair<std::vector<T>, std::vector<T>> ret;
    for (const auto& i : vec) {
        if (partition.func(i)) {
            ret.first.push_back(i);
        } else {
            ret.second.push_back(i);
        }
    }
    return ret;
}

template <typename T, typename Func>
auto operator|(const std::vector<T>& vec, const GroupBy<Func>& group_by) {
    using Key = decltype(group_by.func(T()));
    if constexpr (UnorderedHashable<Key>) {
        UnorderedMap<Key, std::vector<T>> groups;
        for (const auto& i : vec) {
            const auto key = group_by.func(i);
            groups[key].push_back(i);
        }
        return groups;
    } else {
        std::map<Key, std::vector<T>> groups;
        for (const auto& i : vec) {
            const auto key = group_by.func(i);
            groups[key].push_back(i);
        }
        return groups;
    }
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
    std::vector<Ret> ret;
    ret.reserve(std::size(vec) + 1);
    ret.push_back(running_fold.init);
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

template <typename T, typename Cmp>
auto operator|(
    const std::vector<T>& vec, [[maybe_unused]] const Sorted<Cmp>& sorted
) {
    auto ret = vec;
    std::sort(std::begin(ret), std::end(ret));
    return ret;
}

template <typename T>
auto operator|(
    const std::vector<T>& vec, [[maybe_unused]] const Unique& unique
) {
    auto ret = vec;
    std::sort(std::begin(ret), std::end(ret));
    ret.erase(std::unique(std::begin(ret), std::end(ret)), std::end(ret));
    return ret;
}

template <typename T>
auto operator|(
    const std::vector<T>& vec, [[maybe_unused]] const WithIndex& with_index
) {
    return zip(index_range_of(vec), vec);
}

} // namespace Functional

inline constexpr auto filter = Functional::FilterBuilder();
inline constexpr auto partition_by = Functional::PartitionBuilder();
inline constexpr auto group_by = Functional::GroupByBuilder();
inline constexpr auto fold = Functional::FoldBuilder();
inline constexpr auto running_fold = Functional::RunningFoldBuilder();
inline constexpr auto reduce = Functional::ReduceBuilder();
inline constexpr auto to_sorted_with = Functional::SortedBuilder();
inline constexpr auto to_sorted_desc = to_sorted_with(std::greater<>());
inline constexpr auto to_sorted = to_sorted_with();
inline constexpr auto to_unique = Functional::Unique();
inline constexpr auto with_index = Functional::WithIndex();

template <typename T> std::vector<T> int_range(const T lo, const T hi) {
    debug_assert(lo <= hi, "cannot create an empty range");
    std::vector<T> ret(hi - lo + 1);
    std::iota(std::begin(ret), std::end(ret), lo);
    return ret;
}

template <typename R, typename T>
std::vector<R> index_range_of(const std::vector<T>& vec) {
    if (ssize(vec) == 0) {
        return {};
    }
    return int_range<R>(0, ssize(vec) - 1);
}

template <typename T>
std::vector<i32> index_range_of_i32(const std::vector<T>& vec) {
    return index_range_of<i32>(vec);
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
