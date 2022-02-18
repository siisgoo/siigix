#ifndef BYTEORDER_HPP_BKU8P9BT
#define BYTEORDER_HPP_BKU8P9BT

#ifdef _WIN32_
    #include <winsock.h>
#elif __linux__
    #include <netinet/in.h>
#endif

#include "siigix/General/Types.hpp"

#include <algorithm>
#include <stdlib.h>

namespace sgx
{
    class ByteOrder {
        public:
            static sgx::uint16 toNetwork(sgx::uint16);
            static sgx::uint32 toNetwork(sgx::uint32);

            static sgx::uint16 toHost(sgx::uint16);
            static sgx::uint32 toHost(sgx::uint32);

            static int16 flipBytes(int16 value);
            static uint16 flipBytes(uint16 value);
            static int32 flipBytes(int32 value);
            static uint32 flipBytes(uint32 value);
            static float flipBytes(float value);
            static double flipBytes(double value);
            static int64 flipBytes(int64 value);
            static uint64 flipBytes(uint64 value);

            virtual ~ByteOrder() {  }
        private:
            ByteOrder() {  }

            template<typename T>
            static T flip(T value)
            {
                T flip = value;
                std::size_t halfSize = sizeof(T) / 2;
                char* flipP = reinterpret_cast<char*>(&flip);

                for (std::size_t i = 0; i < halfSize; i++)
                {
                    std::swap(flipP[i], flipP[sizeof(T) - i - 1]);
                }
                return flip;
            }
    };

    inline sgx::uint16 ByteOrder::toNetwork(sgx::uint16 hostb) {
        return htons(hostb);
    }

    inline sgx::uint32 ByteOrder::toNetwork(sgx::uint32 hostb) {
        return htonl(hostb);
    }

    inline sgx::uint16 ByteOrder::toHost(sgx::uint16 netwb) {
        return ntohs(netwb);
    }

    inline sgx::uint32 ByteOrder::toHost(sgx::uint32 netwb) {
        return ntohl(netwb);
    }

    inline uint16 ByteOrder::flipBytes(uint16 value)
    {
        return ((value >> 8) & 0x00FF) | ((value << 8) & 0xFF00);
    }


    inline int16 ByteOrder::flipBytes(int16 value)
    {
        return int16(flipBytes(uint16(value)));
    }


    inline uint32 ByteOrder::flipBytes(uint32 value)
    {
        return ((value >> 24) & 0x000000FF) | ((value >> 8) & 0x0000FF00)
            | ((value << 8) & 0x00FF0000) | ((value << 24) & 0xFF000000);
    }


    inline int32 ByteOrder::flipBytes(int32 value)
    {
        return int32(flipBytes(uint32(value)));
    }


    inline float ByteOrder::flipBytes(float value)
    {
        return flip(value);
    }


    inline double ByteOrder::flipBytes(double value)
    {
        return flip(value);
    }


    inline uint64 ByteOrder::flipBytes(uint64 value)
    {
        uint32 hi = uint32(value >> 32);
        uint32 lo = uint32(value & 0xFFFFFFFF);
        return uint64(flipBytes(hi)) | (uint64(flipBytes(lo)) << 32);
    }

    inline int64 ByteOrder::flipBytes(int64 value)
    {
        return int64(flipBytes(uint64(value)));
    }

} /* sgx */ 

#endif /* end of include guard: BYTEORDER_HPP_BKU8P9BT */
