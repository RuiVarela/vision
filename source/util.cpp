#include "vs.hpp"

namespace vs
{

std::string toNativeSeparators(const std::string &path)
{
    std::string output = path;

#ifdef __linux__

#elif _WIN32
    std::replace(output.begin(), output.end(), '\\', '/');
#endif

    return output;
}


} // namespace vs
