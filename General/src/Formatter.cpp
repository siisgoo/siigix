#include "siigix/General/Formatter.hpp"

#include <algorithm>

namespace sgx
{
    std::string
    Formatter::toLower(const std::string& str)
    {
        std::string ret(str);

        std::transform(ret.begin(), ret.end(), ret.begin(),
            [](unsigned char c){ return std::tolower(c);  });

        return ret;
    }

    std::string
    Formatter::toUpper(const std::string& str)
    {
        std::string ret(str);

        std::transform(ret.begin(), ret.end(), ret.begin(),
            [](unsigned char c){ return std::toupper(c);  });

        return ret;
    }
} /* sgx */ 
