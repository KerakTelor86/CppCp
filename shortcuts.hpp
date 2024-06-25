#ifndef CPPCP_SHORTCUTS

#define LENGTH_INNER(_8, _7, _6, _5, _4, _3, _2, _1, N, ...) N
#define LENGTH(...) LENGTH_INNER(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define CONCAT_INNER(a, b) a##b
#define CONCAT(a, b) CONCAT_INNER(a, b)

#define EXCEPT_LAST_INNER_1(...)
#define EXCEPT_LAST_INNER_2(_0, ...) _0
#define EXCEPT_LAST_INNER_3(_0, _1, ...) _0, _1
#define EXCEPT_LAST_INNER_4(_0, _1, _2, ...) _0, _1, _2
#define EXCEPT_LAST_INNER_5(_0, _1, _2, _3, ...) _0, _1, _2, _3
#define EXCEPT_LAST_INNER_6(_0, _1, _2, _3, _4, ...) _0, _1, _2, _3, _4
#define EXCEPT_LAST_INNER_7(_0, _1, _2, _3, _4, _5, ...) _0, _1, _2, _3, _4, _5
#define EXCEPT_LAST_INNER_8(_0, _1, _2, _3, _4, _5, _6, ...) \
    _0, _1, _2, _3, _4, _5, _6
#define EXCEPT_LAST(...) \
    CONCAT(EXCEPT_LAST_INNER_, LENGTH(__VA_ARGS__))(__VA_ARGS__)

#define LAST_OF_INNER_1(_0, ...) _0
#define LAST_OF_INNER_2(_0, _1, ...) _1
#define LAST_OF_INNER_3(_0, _1, _2, ...) _2
#define LAST_OF_INNER_4(_0, _1, _2, _3, ...) _3
#define LAST_OF_INNER_5(_0, _1, _2, _3, _4, ...) _4
#define LAST_OF_INNER_6(_0, _1, _2, _3, _4, _5, ...) _5
#define LAST_OF_INNER_7(_0, _1, _2, _3, _4, _5, _6, ...) _6
#define LAST_OF_INNER_8(_0, _1, _2, _3, _4, _5, _6, _7, ...) _7
#define LAST_OF(...) CONCAT(LAST_OF_INNER_, LENGTH(__VA_ARGS__))(__VA_ARGS__)

#define lambda(...) \
    [&](EXCEPT_LAST(__VA_ARGS__)) { return LAST_OF(__VA_ARGS__); }
#define typed_lambda(return_type, ...)             \
    [&](EXCEPT_LAST(__VA_ARGS__)) -> return_type { \
        return LAST_OF(__VA_ARGS__);               \
    }

#define _ CONCAT(_ignored_variable_, __COUNTER__)
#define __ [[maybe_unused]] const auto& _

#endif
