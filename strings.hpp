#ifndef CPPCP_STRINGS
#define CPPCP_STRINGS

#include <string>
#include <vector>

#include "types.hpp"

namespace CppCp {

inline std::vector<string> split(const string& s, char delim = ' ') {
    std::vector<string> ret;
    string buffer;
    for (const auto& i : s) {
        if (i == delim) {
            if (buffer.length() > 0) {
                ret.push_back(std::move(buffer));
                buffer = "";
            }
        } else {
            buffer += i;
        }
    }
    if (buffer.length() > 0) {
        ret.push_back(std::move(buffer));
    }
    return ret;
}

} // namespace CppCp

#endif
