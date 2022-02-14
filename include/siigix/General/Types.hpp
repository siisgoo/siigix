#ifndef TYPES_HPP_I4HURKXF
#define TYPES_HPP_I4HURKXF

#if defined(__linux__)
#include <sys/types.h>
#else
#include <Windows.h>
#endif

#include <cinttypes>

namespace sgx {
    #if defined(__linux__)
        using int8    = std::int8_t;
        using uint8   = std::uint8_t;
        using int16   = std::int16_t;
        using uint16  = std::uint16_t;
        using int32   = std::int32_t;
        using uint32  = std::uint32_t;
        using int64   = std::int64_t;
        using uint64  = std::uint64_t;
        using intPtr  = std::intptr_t;
        using uintPtr = std::uintptr_t;
        using Byte    = uint8;
    #else

    #endif
} /* sgx  */ 

#endif /* end of include guard: TYPES_HPP_I4HURKXF */
