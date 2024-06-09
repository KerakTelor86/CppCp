#ifndef CPPCP_TYPES
#define CPPCP_TYPES

#include <iostream>
#include <string>
#include <vector>

namespace CppCp {

using i8 = char;
using i16 = short;
using i32 = int;
using i64 = long long;
using i128 = __int128;

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned;
using u64 = unsigned long long;
using usize = size_t;

using f32 = float;
using f64 = double;
using f80 = long double;

using string = std::string;

}; // namespace CppCp

#endif
