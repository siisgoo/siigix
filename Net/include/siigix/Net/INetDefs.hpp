#ifndef INETDEFS_HPP_UMTXQ5ZW
#define INETDEFS_HPP_UMTXQ5ZW

#include "siigix/General/Types.hpp"
#include <vector>

#ifdef __linux__
    #include <sys/ioctl.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <sys/un.h>
    #include <sys/uio.h>
    #include <netinet/in.h>
    #include <netinet/ip6.h>
    #include <netinet/ip.h>
    #include <net/if.h>
    #include <netinet/tcp.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <errno.h>

    #define MAX_CONN 10
    #define SGX_INVALID_SOCK -1
    #define sgx_socket_t int
    #define sgx_socklen_t socklen_t
    #define sgx_ioctl_request_t int

    #define sgx_closesocket(s)  ::close(s)

    #ifndef ADDRESS_FAMILY
        #define ADDRESS_FAMILY int
    #endif

    /* ERRNO REDEFENITION */
    #define SGX_EINTR           EINTR
    #define SGX_EACCES          EACCES
    #define SGX_EFAULT          EFAULT
    #define SGX_EINVAL          EINVAL
    #define SGX_EMFILE          EMFILE
    #define SGX_EAGAIN          EAGAIN
    #define SGX_EWOULDBLOCK     EWOULDBLOCK
    #define SGX_EINPROGRESS     EINPROGRESS
    #define SGX_EALREADY        EALREADY
    #define SGX_ENOTSOCK        ENOTSOCK
    #define SGX_EDESTADDRREQ    EDESTADDRREQ
    #define SGX_EMSGSIZE        EMSGSIZE
    #define SGX_EPROTOTYPE      EPROTOTYPE
    #define SGX_ENOPROTOOPT     ENOPROTOOPT
    #define SGX_EPROTONOSUPPORT EPROTONOSUPPORT
    #if defined(ESOCKTNOSUPPORT)
        #define SGX_ESOCKTNOSUPPORT ESOCKTNOSUPPORT
    #else
        #define SGX_ESOCKTNOSUPPORT -1
    #endif
    #define SGX_ENOTSUP         ENOTSUP
    #define SGX_EPFNOSUPPORT    EPFNOSUPPORT
    #define SGX_EAFNOSUPPORT    EAFNOSUPPORT
    #define SGX_EADDRINUSE      EADDRINUSE
    #define SGX_EADDRNOTAVAIL   EADDRNOTAVAIL
    #define SGX_ENETDOWN        ENETDOWN
    #define SGX_ENETUNREACH     ENETUNREACH
    #define SGX_ENETRESET       ENETRESET
    #define SGX_ECONNABORTED    ECONNABORTED
    #define SGX_ECONNRESET      ECONNRESET
    #define SGX_ENOBUFS         ENOBUFS
    #define SGX_EISCONN         EISCONN
    #define SGX_ENOTCONN        ENOTCONN
    #if defined(ESHUTDOWN)
        #define SGX_ESHUTDOWN   ESHUTDOWN
    #else
        #define SGX_ESHUTDOWN   -2
    #endif
    #define SGX_ETIMEDOUT       ETIMEDOUT
    #define SGX_ECONNREFUSED    ECONNREFUSED
    #if defined(EHOSTDOWN)
        #define SGX_EHOSTDOWN   EHOSTDOWN
    #else
        #define SGX_EHOSTDOWN   -3
    #endif
    #define SGX_EHOSTUNREACH    EHOSTUNREACH
    #define SGX_ESYSNOTREADY    -4
    #define SGX_ENOTINIT        -5
    #define SGX_HOST_NOT_FOUND  HOST_NOT_FOUND
    #define SGX_TRY_AGAIN       TRY_AGAIN
    #define SGX_NO_RECOVERY     NO_RECOVERY
    #define SGX_NO_DATA         NO_DATA

#else /*_WIN32_*/
    #include <windows.h>
    #ifdef __MINGW32__
        #include <Winsock2.h>
        #include <Iphlpapi.h>
        #include <ws2tcpip.h>
    #endif // __MINGW32__
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <ws2def.h>

    #define SGX_INVALID_SOCKET  INVALID_SOCKET
    #define sgx_socket_t        SOCKET
    #define sgx_socklen_t       int
    #define sgx_ioctl_request_t int
    #define sgx_closesocket(s)  closesocket(s)

    #ifndef ADDRESS_FAMILY
        #define ADDRESS_FAMILY USHORT
    #endif

    /* ERRNO */
    #define SGX_EINTR           WSAEINTR
    #define SGX_EACCES          WSAEACCES
    #define SGX_EFAULT          WSAEFAULT
    #define SGX_EINVAL          WSAEINVAL
    #define SGX_EMFILE          WSAEMFILE
    #define SGX_EAGAIN          WSAEWOULDBLOCK
    #define SGX_EWOULDBLOCK     WSAEWOULDBLOCK
    #define SGX_EINPROGRESS     WSAEINPROGRESS
    #define SGX_EALREADY        WSAEALREADY
    #define SGX_ENOTSOCK        WSAENOTSOCK
    #define SGX_EDESTADDRREQ    WSAEDESTADDRREQ
    #define SGX_EMSGSIZE        WSAEMSGSIZE
    #define SGX_EPROTOTYPE      WSAEPROTOTYPE
    #define SGX_ENOPROTOOPT     WSAENOPROTOOPT
    #define SGX_EPROTONOSUPPORT WSAEPROTONOSUPPORT
    #define SGX_ESOCKTNOSUPPORT WSAESOCKTNOSUPPORT
    #define SGX_ENOTSUP         WSAEOPNOTSUPP
    #define SGX_EPFNOSUPPORT    WSAEPFNOSUPPORT
    #define SGX_EAFNOSUPPORT    WSAEAFNOSUPPORT
    #define SGX_EADDRINUSE      WSAEADDRINUSE
    #define SGX_EADDRNOTAVAIL   WSAEADDRNOTAVAIL
    #define SGX_ENETDOWN        WSAENETDOWN
    #define SGX_ENETUNREACH     WSAENETUNREACH
    #define SGX_ENETRESET       WSAENETRESET
    #define SGX_ECONNABORTED    WSAECONNABORTED
    #define SGX_ECONNRESET      WSAECONNRESET
    #define SGX_ENOBUFS         WSAENOBUFS
    #define SGX_EISCONN         WSAEISCONN
    #define SGX_ENOTCONN        WSAENOTCONN
    #define SGX_ESHUTDOWN       WSAESHUTDOWN
    #define SGX_ETIMEDOUT       WSAETIMEDOUT
    #define SGX_ECONNREFUSED    WSAECONNREFUSED
    #define SGX_EHOSTDOWN       WSAEHOSTDOWN
    #define SGX_EHOSTUNREACH    WSAEHOSTUNREACH
    #define SGX_ESYSNOTREADY    WSASYSNOTREADY
    #define SGX_ENOTINIT        WSANOTINITIALISED
    #define SGX_HOST_NOT_FOUND  WSAHOST_NOT_FOUND
    #define SGX_TRY_AGAIN       WSATRY_AGAIN
    #define SGX_NO_RECOVERY     WSANO_RECOVERY
    #define SGX_NO_DATA         WSANO_DATA
#endif

#define sgx_set_sa_len(pSA, len) (pSA)->sa_len = (len)
#define sgx_set_sin_len(pSA)     (pSA)->sin_len = sizeof(struct sockaddr_in)
#define poco_set_sin6_len(pSA)   (pSA)->sin6_len = sizeof(struct sockaddr_in6)

#ifdef __linux__
    #define sgx_set_sun_len(pSA, len) (pSA)->sun_len = (len)
#endif

/* RESERVED ADRESSESS */
#ifndef INADDR_NONE
    #define INADDR_NONE 0xffffffff
#endif

#ifndef INADDR_ANY
    #define INADDR_ANY 0x00000000
#endif

#ifndef INADDR_BROADCAST
    #define INADDR_BROADCAST 0xffffffff
#endif

#ifndef INADDR_LOOPBACK
    #define INADDR_LOOPBACK 0x7f000001
#endif

#ifndef INADDR_UNSPEC_GROUP
    #define INADDR_UNSPEC_GROUP 0xe0000000
#endif

#ifndef INADDR_ALLHOSTS_GROUP
    #define INADDR_ALLHOSTS_GROUP 0xe0000001
#endif

#ifndef INADDR_ALLRTRS_GROUP
    #define INADDR_ALLRTRS_GROUP 0xe0000002
#endif

#ifndef INADDR_MAX_LOCAL_GROUP
    #define INADDR_MAX_LOCAL_GROUP 0xe00000ff
#endif

namespace sgx {
    namespace Net {
        /* SocketBuf */
        #if defined(_WIN32_)
            typedef WSABUF SocketBuf;
        #elif defined(__linux__)
            typedef iovec SocketBuf;
        #endif

        typedef std::vector<SocketBuf> SocketBufVec;

        typedef sgx::uint16 port_t;
        typedef sgx::uint32 ip4_t;
        typedef in6_addr ip6_t;

        typedef int ka_prop_t;
        struct KeepAliveConfig{
            ka_prop_t ka_idle  = 120;
            ka_prop_t ka_intvl = 3;
            ka_prop_t ka_cnt   = 5;
        };

        enum AddressFamily {
            IPv4,
            IPv6,

            #if defined(__linux__)
            UNIX_LOCAL,
            #endif
        };
    } /* Net */ 
} /* sgx */ 

#endif /* end of include guard: INETDEFS_HPP_UMTXQ5ZW */
