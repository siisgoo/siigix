#ifndef UTILS_HPP_RX3OUY7G
#define UTILS_HPP_RX3OUY7G

#include <string>
#include <sstream>
#include <utility>
#include <cstddef>

namespace siigix {

    template<typename... Args>
    int print(std::ostream& s, Args&... args)
    {
        using Expander = int[];
        return Expander{ 0, ((s << std::forward<Args>(args)), 0)... }[0];
    }

    template<typename... Args>
    std::string buildStringFromParts(Args const&... args)
    {
        std::stringstream msg;
        print(msg, args...);
        return msg.str();
    }

    template<typename... Args>
    std::string buildErrorMessage(Args const&... args)
    {
        return buildStringFromParts(args...);
    }

} /* siigix */

#endif /* end of include guard: UTILS_HPP_RX3OUY7G */
