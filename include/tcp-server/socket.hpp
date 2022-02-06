#ifndef SOCKET_HPP_LEMCELXN
#define SOCKET_HPP_LEMCELXN

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "bitflag.hpp"
#include "iobuff.hpp"

namespace INet //put to another ns like VERYGOODNETWORKNS
{
    // VERY BIG good job have fun suck pip like lisp jeeeeeeeeeeeeeeeez
    class Socket {
        public:
            enum Status : bitflag8_t {
                up = 1,
                closed = 1 << 1,
                ERR_BIND   = 1 << 2,
                ERR_LISTEN = 1 << 3,
                ERR_CREATE = 1 << 4,
            } status;

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

            bool IsOK();
        private:
            int _fd;
    };
} /* TPC */ 

#endif /* end of include guard: SOCKET_HPP_LEMCELXN */
