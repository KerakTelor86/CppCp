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

template <typename Func> struct RunningReduce {
    constexpr RunningReduce(const Func& _func) : func(_func) {}
    const Func func;
};

template <typename Tr> struct Transform {
    constexpr Transform(const Tr& _tr) : tr(_tr) {}
    const Tr tr;
};

template <typename Cmp> struct Sorted {
    constexpr Sorted() {}
};

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

struct RunningReduceBuilder {
    constexpr auto operator()(const auto& func) const {
        return RunningReduce(func);
    }
};

struct TransformBuilder {
    constexpr auto operator()(const auto& tr) const {
        return Transform(tr);
    }
};

struct SortedBuilder {
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

template <typename T, typename Func>
auto operator|(
    const std::vector<T>& vec, const RunningReduce<Func>& running_reduce
) {
    using Ret = decltype(running_reduce.func(T(), T()));
    std::vector<Ret> ret;
    ret.reserve(std::size(vec));
    ret.push_back(vec[0]);
    for (i32 i = 1; i < ssize(vec); ++i) {
        ret.push_back(running_reduce.func(ret.back(), vec[i]));
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

template <typename T, typename Tr>
auto operator|(
    const std::vector<T>& vec, [[maybe_unused]] const Transform<Tr>& transform
) {
    return transform.tr(vec);
}

} // namespace Functional

inline constexpr auto filter = Functional::FilterBuilder();
inline constexpr auto partition_by = Functional::PartitionBuilder();
inline constexpr auto group_by = Functional::GroupByBuilder();
inline constexpr auto fold_with = Functional::FoldBuilder();
inline constexpr auto running_fold_with = Functional::RunningFoldBuilder();
inline constexpr auto reduce_with = Functional::ReduceBuilder();
inline constexpr auto running_reduce_with = Functional::RunningReduceBuilder();

inline constexpr auto to_sorted_with = Functional::SortedBuilder();
inline constexpr auto to_sorted_desc = to_sorted_with(std::greater<>());
inline constexpr auto to_sorted = to_sorted_with(std::less<>());

inline constexpr auto transform_with = Functional::TransformBuilder();

inline constexpr auto to_reversed = transform_with([](auto vec) {
    std::reverse(std::begin(vec), std::end(vec));
    return vec;
});
inline constexpr auto to_unique = transform_with([](auto vec) {
    std::sort(std::begin(vec), std::end(vec));
    vec.erase(std::unique(std::begin(vec), std::end(vec)), std::end(vec));
    return vec;
});
inline constexpr auto to_unzipped = transform_with([](const auto& vec) {
    return unzip(vec);
});

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

inline constexpr auto with_index = transform_with([](const auto& vec) {
    return zip(index_range_of(vec), vec);
});
inline constexpr auto with_index_i32 = transform_with([](const auto& vec) {
    return zip(index_range_of_i32(vec), vec);
});

template <typename T>
auto pipe_vec(const std::vector<T>& vec, const auto&... funcs) {
    using Functional::operator|;
    return (vec | ... | funcs);
}

template <typename T, usize Len>
auto pipe_vec(const std::array<T, Len>& arr, const auto&... funcs) {
    return pipe_vec(std::vector(std::begin(arr), std::end(arr)), funcs...);
}

auto pipe_vec(const string& str, const auto&... funcs) {
    return pipe_vec(std::vector(std::begin(str), std::end(str)), funcs...);
}

} // namespace CppCp

#endif
