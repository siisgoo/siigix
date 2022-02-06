#ifndef SOCKET_HPP_LEMCELXN
#define SOCKET_HPP_LEMCELXN

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#include <map>
#include <string>

#include "BitFlag.hpp"
#include "IOBuff.hpp"

static const int g_sock_errno_count = 29; //add somethig better realization of defenition
#define SOCKET_ERRNO_MAP(XX) \
    /* socket() errors */ \
    XX( EAFNOSUPPORT,    ERR_AFNOSUPPORT,    "The implementation does not support the specified address family" ) \
    XX( EMFILE,          ERR_MFILE,          "All file descriptors available to the process are currently open" ) \
    XX( ENFILE,          ERR_NFILE,          "No more file descriptors are available for the system" ) \
    XX( EPROTONOSUPPORT, ERR_PROTONOSUPPORT, "The protocol is not supported by the address family or the protocol is not supported by the implemen‐tation" ) \
    XX( EPROTOTYPE,      ERR_PROTOTYPE,      "The socket type is not supported by the protocol" ) \
    XX( EACCES,          ERR_ACCES,          "The process does not have appropriate privileges" ) \
    XX( ENOBUFS,         ERR_NOBUFS,         "Insufficient resources were available in the system to perform the operation" ) \
    XX( ENOMEM,          ERR_NOMEM,          "Insufficient memory was available to fulfill the request" ) \
    /* connect() errors */ \
    /* The connect() function shall fail if: */ \
    XX( EADDRNOTAVAIL,   ERR_ADDRNOTAVAIL,   "The specified address is not available from the local machine." ) \
    /* XX( EAFNOSUPPORT, CON_AFNOSUPPORT,"The specified address is not a valid address for the address family of the specified socket." ) \ */ \
    XX( EALREADY,        ERR_ALREADY,        "A connection request is already in progress for the specified socket." ) \
    XX( EBADF,           ERR_BADF,           "The socket argument is not a valid file descriptor." ) \
    XX( ECONNREFUSED,    ERR_CONNREFUSED,    "The target address was not listening for connections or refused the connection request." ) \
    XX( EINPROGRESS,     ERR_INPROGRESS,     "O_NONBLOCK  is set for the file descriptor for the socket and the connection cannot be immediately es‐tablished; the connection shall be established asynchronously." ) \
    XX( EINTR,           ERR_INTR,           "The attempt to establish a connection was interrupted by delivery of a signal  that  was  caught;  the connection shall be established asynchronously." ) \
    XX( EISCONN,         ERR_ISCONN,         "The specified socket is connection-mode and is already connected." ) \
    XX( ENETUNREACH,     ERR_NETUNREACH,     "No route to the network is present." ) \
    XX( ENOTSOCK,        ERR_NOTSOCK,        "The socket argument does not refer to a socket." ) \
    /* XX( EPROTOTYPE,   ERR_   CON_PROTOTYPE,  "The specified address has a different type than the socket bound to the specified peer address." ) \ */ \
    XX( ETIMEDOUT,       ERR_TIMEDOUT,       "The attempt to connect timed out before a connection was made." ) \
    /* If the address family of the socket is AF_UNIX, then connect() shall fail if: */ \
    XX( EIO,             ERR_IO,             "An I/O error occurred while reading from or writing to the file system." ) \
    XX( ELOOP,           ERR_LOOP,           "A loop exists in symbolic links encountered during resolution of the pathname in address." ) \
    XX( ENAMETOOLONG,    ERR_NAMETOOLONG,    "The length of a component of a pathname is longer than {NAME_MAX}." ) \
    XX( ENOENT,          ERR_NOENT,          "A component of the pathname does not name an existing file or the pathname is an empty string." ) \
    XX( ENOTDIR,         ERR_NOTDIR,         "A component of the path prefix of the pathname in address names an existing file that is neither a di‐ rectory nor a symbolic link to a  directory  or  the  pathname  in  address  contains  at  least  one non-<slash> character and ends with one or more trailing <slash> characters and the last pathname com‐ ponent names an existing file that is neither a directory nor a symbolic link to a directory." ) \
    /* The connect() function may fail if: */ \
    /* XX( EACCES,          ERR_CON_ACCES,      "Search permission is denied for a component of the path prefix; or write access to the named socket is denied." ) \ */ \
    XX( EADDRINUSE,      ERR_ADDRINUSE,      "Attempt to establish a connection that uses addresses that are already in use." ) \
    XX( ECONNRESET,      ERR_CONNRESET,      "Remote host reset the connection request." ) \
    XX( EHOSTUNREACH,    ERR_HOSTUNREACH,    "The  destination  host  cannot be reached." ) \
    XX( EINVAL,          ERR_INVAL,          "The address_len argument is not a valid length for the address family or invalid  address  family  in the sockaddr structure." ) \
    /* XX( ELOOP,        ERR_   LOOP,           "More than {SYMLOOP_MAX} symbolic links were encountered during resolution of the pathname in address." ) \ */ \
    /* XX( ENAMETOOLONG, ERR_   NAMETOOLONG,    "The length of a pathname exceeds {PATH_MAX} or pathname resolution of a symbolic link produced an in‐termediate result with a length that exceeds {PATH_MAX}." ) \ */ \
    XX( ENETDOWN,        ERR_NETDOWN,        "The local network interface used to reach the destination is down." ) \
    /* XX( ENOBUFS,      ERR_   CON_NOBUFS,     "No buffer space is available." ) \ */ \
    XX( EOPNOTSUPP,      ERR_OPNOTSUPP,      "The socket is listening and cannot be connected." ) \

namespace siigix {
    namespace INet {
        typedef __uint16_t port_t;
        typedef __uint32_t ip4_t;
        typedef in6_addr   ip6_t;

        /* object for contaion ip4 or ip6 addr */
        union usockaddr_in_any {
            struct sockaddr_in v4;
            struct sockaddr_in6 v6;
            usockaddr_in_any(struct sockaddr_in v) : v4(v) {}
            usockaddr_in_any(struct sockaddr_in6 v) : v6(v) {}
            usockaddr_in_any() {}
            ~usockaddr_in_any() {}
        };

        //naxui eto? may chota luchshe(improve or delete)?
        typedef struct sockaddr_in_any sockaddr_in_any;
        struct sockaddr_in_any {
            private:
                union usockaddr_in_any _sockaddr_in;
                bool _is_v6;
            public:
                sockaddr_in_any() : _is_v6(false) { }
                sockaddr_in_any(bool is_v6) : _is_v6(is_v6) { }
                sockaddr_in_any(struct sockaddr_in sai) :
                    _is_v6(false), _sockaddr_in(sai)
                {  }
                sockaddr_in_any(struct sockaddr_in6 sai6) :
                    _is_v6(true), _sockaddr_in(sai6)
                {  }

                void use_v6(bool use) { _is_v6 = use; }
                bool is_v6() const    { return _is_v6; }

                struct sockaddr_in  &ip4() { return _sockaddr_in.v4; };
                struct sockaddr_in6 &ip6() { return _sockaddr_in.v6; };
                const struct sockaddr_in  ip4() const { return _sockaddr_in.v4; };
                const struct sockaddr_in6 ip6() const { return _sockaddr_in.v6; };
        };

        /* return count of founded ips and ip+interfacename map */
        int getLocalIP(std::map<std::string, in_addr_t>& ret);
        /* network addresses comparsing */
        bool operator==(struct sockaddr_in, struct sockaddr_in);
        bool operator==(struct sockaddr_in6 a, struct sockaddr_in6 b);
        bool operator==(const sockaddr_in_any a, const sockaddr_in_any b);

        // VERY BIG good job have fun suck pip like lisp jeeeeeeeeeeeeeeeez
        class Socket {
            public:
                #define XX(val, name, string) name = val,
                enum Errno {
                    SOCKET_ERRNO_MAP(XX)
                };
                #undef XX

                static const std::map<int, const char*> errno_with_desk;

            public:
                /* create new */
                Socket(int domain, int type, int protocol);
                /* create copy */
                explicit Socket(int fd);
                virtual ~Socket();

                bool Close();

                bool Bind(const struct sockaddr *addr_to_bind, socklen_t addr_len);

                int  Accept(struct sockaddr *remote_host_addr_ret, socklen_t *addr_len);
                int  Accept4(struct sockaddr *remote_host_addr_ret, socklen_t *addr_len, int flags);
                bool Listen(int maxconn);

                bool GetName(struct sockaddr *ret_addr, socklen_t *addr_len);
                bool GetPeerName(struct sockaddr *remote_host_addr_ret, socklen_t *addr_len);

                bool Connect(struct sockaddr *connect_to_addr, socklen_t addr_len);
                bool Connect(sockaddr_in_any *connect_to_addr);
                bool Connect(int family, const char *ip, __uint16_t port);
                bool Shutdown(int shutdown_type = SHUT_RDWR);

                bool Send(const IOBuff& buff, int flags = 0) const;
                bool Recv(IOBuff& recvto, size_t len, int flags = 0);
                /* bool sendTo(); */
                /* bool recvFrom(); */
                /* bool sendMsg(); */
                /* bool recvMsg(); */

                bool GetOpts(int level, int option, void *value, socklen_t *opt_len) const;
                bool SetOpts(int level, int option, const int *value, socklen_t opt_len);

                bool IsUP() const { return _fd; };
                const char* errno_to_human() const;
                static const char *errno_to_human(int custom_errno);
            private:
                int _fd;
                int _errno;
        };
    } /* TPC */ 

} /* siigix */ 

#endif /* end of include guard: SOCKET_HPP_LEMCELXN */
