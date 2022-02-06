#ifndef SOCKET_HPP_LEMCELXN
#define SOCKET_HPP_LEMCELXN

//TODO remove some
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <map>

#include "BitFlag.hpp"
#include "IOBuff.hpp"

namespace siigix {
    namespace INet {
        typedef __uint16_t port_t;
        typedef __uint32_t ip4_t;
        typedef in6_addr ip6_t;

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

        std::map<std::string, in_addr_t> getLocalIP();
        bool operator==(struct sockaddr_in, struct sockaddr_in);
        bool operator==(struct sockaddr_in6 a, struct sockaddr_in6 b);
        bool operator==(const sockaddr_in_any a, const sockaddr_in_any b);

        // VERY BIG good job have fun suck pip like lisp jeeeeeeeeeeeeeeeez
        class Socket {
            public:
                enum Status : bitflag8_t {
                    up = 1,
                    closed = 1 << 1,
                    ERR_BIND   = 1 << 2,
                    ERR_LISTEN = 1 << 3,
                    ERR_CREATE = 1 << 4,
                };

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
                bool Shutdown(int shutdown_type);

                bool Send(const IOBuff& buff, int flags) const;
                bool Recv(IOBuff& recvto, size_t len, int flags);
                /* bool sendTo(); */
                /* bool recvFrom(); */
                /* bool sendMsg(); */
                /* bool recvMsg(); */

                bool GetOpts(int level, int option, void *value, socklen_t *opt_len);
                bool SetOpts(int level, int option, const int *value, socklen_t opt_len);

                Status IsUP();
            private:
                int _fd;
                Status _status;
        };
    } /* TPC */ 

} /* siigix */ 

#endif /* end of include guard: SOCKET_HPP_LEMCELXN */
