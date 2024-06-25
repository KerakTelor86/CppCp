#ifndef CPPCP_SHORTCUTS

#define mut auto
#define imut const auto
#define muts(...) mut[__VA_ARGS__]
#define imuts(...) imut[__VA_ARGS__]

#define typedlambda(ret, type) [&]() -> type { return ret; }
#define typedlambda1(ret, type) \
    [&]([[maybe_unused]] mut&& v1) -> type { return ret; }
#define typedlambda2(ret, type) \
    [&]([[maybe_unused]] mut&& v1, mut&& v2) -> type { return ret; }
#define typedlambda3(ret, type) \
    [&]([[maybe_unused]] mut&& v1, mut&& v2, mut&& v3) -> type { return ret; }
#define typedlambda4(ret, type)                                            \
    [&]([[maybe_unused]] mut&& v1, mut&& v2, mut&& v3, mut&& v4) -> type { \
        return ret;                                                        \
    }
#define typedlambda5(ret, type)                                           \
    [&]([[maybe_unused]] mut&& v1, mut&& v2, mut&& v3, mut&& v4, mut&& v5 \
    ) -> type { return ret; }

#define lambda(ret) typedlambda(ret, auto)
#define lambda1(ret) typedlambda1(ret, auto)
#define lambda2(ret) typedlambda2(ret, auto)
#define lambda3(ret) typedlambda3(ret, auto)
#define lambda4(ret) typedlambda4(ret, auto)
#define lambda5(ret) typedlambda5(ret, auto)

#define CONCAT_INNER(a, b) a##b
#define CONCAT(a, b) CONCAT_INNER(a, b)

#define _ CONCAT(_ignored_variable_, __COUNTER__)

#endif
