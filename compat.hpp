#ifndef CPPCP_COMPAT
#define CPPCP_COMPAT

#include "types.hpp"

#if __cplusplus < 202002L
#define ssize(x) ((i64)size(x))
#endif

#endif
