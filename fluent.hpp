#ifndef CPPCP_FLUENT
#define CPPCP_FLUENT

#include <algorithm>
#include <concepts>
#include <iterator>
#include <map>
#include <numeric>
#include <tuple>

#include "concepts.hpp"
#include "debug.hpp"
#include "types.hpp"
#include "unordered.hpp"
#include "zip.hpp"

namespace CppCp {

namespace {

template <usize Idx = 0, typename T, typename... Funcs>
auto get_helper(const T& val, const std::tuple<Funcs...>& funcs) {
    if constexpr (Idx >= sizeof...(Funcs)) {
        return val;
    } else {
        return get_helper<Idx + 1>(std::get<Idx>(funcs)(val), funcs);
    }
}

} // namespace

template <typename Container, typename... PendingMap>
    requires IterableContainer<Container>
class FluentCollection {
private:
    Container store;
    std::tuple<PendingMap...> funcs;

public:
    using StartType = std::remove_const_t<
        std::remove_reference_t<decltype(*begin(Container()))>>;

    using FinalType = decltype(get_helper(StartType(), funcs));

    FluentCollection(const Container& source) : store(source) {}
    FluentCollection(Container&& source) : store(std::move(source)) {}

    auto get() const {
        if constexpr (sizeof...(PendingMap) == 0 && IndexableContainerOf<Container, StartType>) {
            return store;
        } else {
            std::vector<FinalType> ret;
            ret.reserve(std::size(store));
            for (const auto& it : store) {
                ret.push_back(get_helper(it, funcs));
            }
            return ret;
        }
    }

    template <usize Len> std::array<FinalType, Len> get() const {
        debug_assert(
            std::size(store) == Len,
            "requested array length is not equal to structure length"
        );
        std::array<FinalType, Len> ret;
        for (usize idx = 0; const auto& it : store) {
            ret[idx++] = get_helper(it, funcs);
        }
        return ret;
    }

    std::string get_string() const
        requires std::same_as<FinalType, char>
    {
        const auto got = get();
        return std::string(std::begin(got), std::end(got));
    }

    auto get_unzip() const
        requires TupleLike<FinalType>
    {
        return unzip(get());
    }

    template <typename Func>
        requires Lambda<Func, StartType>
    auto map(const Func& func) {
        return FluentCollection<Container, PendingMap..., Func>(
            std::move(store),
            std::tuple_cat(std::move(funcs), std::make_tuple(func))
        );
    }

    template <typename Func>
        requires Lambda<Func, StartType>
    auto map(const Func& func) const {
        return FluentCollection<Container, PendingMap..., Func>(
            store, std::tuple_cat(funcs, std::make_tuple(func))
        );
    }

    auto flush() {
        const auto ret = get();
        return FluentCollection<decltype(ret)>(std::move(ret));
    }

    template <typename Func>
        requires LambdaWithRet<bool, Func, FinalType>
    auto filter(const Func& filter_func) const {
        std::vector<FinalType> ret;
        for (const auto& it : get()) {
            if (filter_func(it)) {
                ret.push_back(it);
            }
        }
        return FluentCollection<decltype(ret)>(std::move(ret));
    }

    template <typename Func>
        requires LambdaWithRet<bool, Func, FinalType>
    auto partition(const Func& filter_func) const {
        std::array<std::vector<FinalType>, 2> ret;
        for (const auto& it : get()) {
            if (filter_func(it)) {
                ret[0].push_back(it);
            } else {
                ret[1].push_back(it);
            }
        }
        return FluentCollection<decltype(ret)>(std::move(ret));
    }

    template <typename Func>
        requires Lambda<Func, FinalType>
    auto group(const Func& key_func) const {
        using Key = decltype(key_func(FinalType()));
        auto map = [] {
            if constexpr (UnorderedHashable<Key>) {
                return UnorderedMap<Key, std::vector<FinalType>>();
            } else {
                return std::map<Key, std::vector<FinalType>>();
            }
        }();
        for (const auto& it : get()) {
            const auto key = key_func(it);
            map[key].push_back(it);
        }
        return FluentCollection<decltype(map)>(std::move(map));
    }

    template <typename Func>
        requires LambdaWithRet<i32, Func, FinalType>
    auto group_ranged(const i32 range, const Func& key_func) const {
        std::vector<std::vector<FinalType>> ret(range);
        for (const auto& it : get()) {
            const auto key = key_func(it);
            ret[key].push_back(it);
        }
        return FluentCollection<decltype(ret)>(std::move(ret));
    }

    template <typename Func>
        requires Lambda<Func, FinalType, FinalType>
    auto reduce(const Func& binary_op) const {
        using Ret = decltype(binary_op(FinalType(), FinalType()));

        const auto got = get();
        Ret ret = got[0];
        for (usize i = 1; i < std::size(got); ++i) {
            ret = binary_op(ret, got[i]);
        }
        return ret;
    }

    template <typename Func>
        requires Lambda<Func, FinalType, FinalType>
    auto running_reduce(const Func& binary_op) const {
        using Ret = decltype(binary_op(FinalType(), FinalType()));

        const auto got = get();

        std::vector<Ret> ret;
        ret.reserve(std::size(got));

        ret.push_back(got[0]);
        for (usize i = 1; i < std::size(got); ++i) {
            ret.emplace_back(binary_op(ret.back(), got[i]));
        }
        return FluentCollection<decltype(ret)>(std::move(ret));
    }

    template <typename Func, typename InitType>
        requires Lambda<Func, InitType, FinalType>
    auto fold(const InitType& init, const Func& binary_op) const {
        InitType ret = init;
        for (const auto& it : get()) {
            ret = binary_op(ret, it);
        }
        return ret;
    }

    template <typename Func, typename InitType>
        requires Lambda<Func, InitType, FinalType>
    auto running_fold(const InitType& init, const Func& binary_op) const {
        const auto got = get();

        std::vector<InitType> ret;
        ret.reserve(std::size(got));

        ret.push_back(init);
        for (const auto& it : got) {
            ret.push_back(std::move(binary_op(ret.back(), it)));
        }
        return FluentCollection<decltype(ret)>(std::move(ret));
    }

    template <typename Func>
        requires Lambda<Func, std::vector<FinalType>>
    auto transform(const Func& transform_func) const {
        auto ret = transform_func(get());
        return FluentCollection<decltype(ret)>(std::move(ret));
    }

    template <typename Cmp> auto sorted_with(const Cmp& cmp) const {
        return transform([&](auto vec) {
            std::sort(std::begin(vec), std::end(vec), cmp);
            return vec;
        });
    }

    auto sorted() const
        requires std::totally_ordered<FinalType>
    {
        return sorted_with(std::less<>());
    }

    auto sorted_desc() const
        requires std::totally_ordered<FinalType>
    {
        return sorted_with(std::greater<>());
    }

    template <typename Func>
    auto sorted_by(const Func& key_func) const
        requires Lambda<Func, FinalType>
                 && std::totally_ordered<decltype(key_func(FinalType()))>
    {
        using Key = decltype(key_func(FinalType()));
        return transform([&](const std::vector<FinalType>& vec) {
            std::vector<std::pair<Key, usize>> order;
            order.reserve(std::size(vec));
            for (usize i = 0; i < std::size(vec); ++i) {
                order.emplace_back(key_func(vec[i]), i);
            }
            std::sort(std::begin(order), std::end(order));
            std::vector<FinalType> ret(std::size(vec));
            for (usize i = 0; i < std::size(vec); ++i) {
                ret[i] = vec[order[i].second];
            }
            return ret;
        });
    }

    auto distinct() const
        requires std::totally_ordered<FinalType>
    {
        return sorted().transform([](std::vector<FinalType> vec) {
            vec.erase(
                std::unique(std::begin(vec), std::end(vec)), std::end(vec)
            );
            return vec;
        });
    }

    template <typename Func>
    auto distinct_by(const Func& key_func) const
        requires Lambda<Func, FinalType>
                 && std::totally_ordered<decltype(key_func(FinalType()))>
    {
        using Key = decltype(key_func(FinalType()));
        return transform([&](const std::vector<FinalType>& vec) {
            std::vector<std::pair<Key, usize>> order;
            order.reserve(std::size(vec));
            for (usize i = 0; i < std::size(vec); ++i) {
                order.emplace_back(key_func(vec[i]), i);
            }
            std::sort(std::begin(order), std::end(order));
            order.erase(
                std::unique(
                    std::begin(order),
                    std::end(order),
                    [](const auto& a, const auto& b) {
                        return a.first == b.first;
                    }
                ),
                std::end(order)
            );
            write_line_debug(order);
            std::vector<FinalType> ret(std::size(order));
            for (usize i = 0; i < std::size(order); ++i) {
                ret[i] = vec[order[i].second];
            }
            return ret;
        });
    }

    auto reversed() const {
        return transform([](auto vec) {
            std::reverse(std::begin(vec), std::end(vec));
            return vec;
        });
    }

    template <typename T = i64, typename Func>
        requires LambdaWithRet<bool, Func, FinalType>
    auto count(const Func& predicate_func) const {
        T count = 0;
        for (const auto& it : get()) {
            count += predicate_func(it);
        }
        return count;
    }

    FinalType max() const
        requires std::totally_ordered<FinalType>
    {
        const auto got = get();
        return *std::max_element(std::begin(got), std::end(got));
    }

    FinalType min() const
        requires std::totally_ordered<FinalType>
    {
        const auto got = get();
        return *std::min_element(std::begin(got), std::end(got));
    }

    template <typename T = FinalType>
    T sum() const
        requires Addable<FinalType>
    {
        return reduce(std::plus<>());
    }

    usize size() const {
        return std::size(store);
    }

private:
    FluentCollection(Container&& _store, std::tuple<PendingMap...>&& _funcs)
        : store(std::move(_store)),
          funcs(std::move(_funcs)) {}

    FluentCollection(
        const Container& _store, const std::tuple<PendingMap...>& _funcs
    )
        : store(_store),
          funcs(_funcs) {}

    template <typename A, typename... B>
        requires IterableContainer<A>
    friend class FluentCollection;
};

inline auto fluent(const auto& collection) {
    return FluentCollection(collection);
}

inline auto fluent(auto& collection) {
    return FluentCollection(collection);
}

inline auto fluent(auto&& collection) {
    return FluentCollection(std::move(collection));
}

template <typename T> inline auto fluent_range(const T lo, const T hi) {
    debug_assert(lo <= hi, "cannot create an empty range");
    std::vector<T> range(hi - lo + 1);
    std::iota(std::begin(range), std::end(range), lo);
    return FluentCollection(std::move(range));
}

template <typename T> inline auto fluent_iota(const T hi) {
    debug_assert(hi > 0, "cannot create an empty range");
    return fluent_range<T>(0, hi - 1);
}

template <typename T = i64> inline auto fluent_index(const auto& collection) {
    return fluent_iota<T>(std::ssize(collection));
}

} // namespace CppCp

#endif
