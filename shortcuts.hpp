#ifndef CPPCP_SHORTCUTS

#define imut const auto
#define mut auto

#define lambda(ret) [&] { return ret; };
#define lambda1(ret) [&]([[maybe_unused]] mut&& v1) { return ret; }
#define lambda2(ret) [&]([[maybe_unused]] mut&& v1, mut&& v2) { return ret; }
#define lambda3(ret) \
    [&]([[maybe_unused]] mut&& v1, mut&& v2, mut&& v3) { return ret; }
#define lambda4(ret) \
    [&]([[maybe_unused]] mut&& v1, mut&& v2, mut&& v3, mut&& v4) { return ret; }
#define lambda5(ret)                                                         \
    [&]([[maybe_unused]] mut&& v1, mut&& v2, mut&& v3, mut&& v4, mut&& v5) { \
        return ret;                                                          \
    }

#endif
