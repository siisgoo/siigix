#include <socket.hpp>

namespace INet
{

    Socket::Socket(int domain, int type, int protocol) :
        _fd(socket(domain, type, protocol))
    { }

    Socket::Socket(int fd) :
        _fd(fd)
    { }

    Socket::~Socket() { Close(); }

    bool
    Socket::Close() { return close(_fd); }

    bool
    Socket::Bind(const struct sockaddr *addr_to_bind, socklen_t addr_len)
    {
        return bind(_fd, addr_to_bind, addr_len);
    }

    int
    Socket::Accept(struct sockaddr *remote_host_addr_ret, socklen_t *addr_len)
    {
        return accept(_fd, remote_host_addr_ret, addr_len);
    }

    int
    Socket::Accept4(struct sockaddr *remote_host_addr_ret, socklen_t *addr_len, int flags)
    {
        return accept4(_fd, remote_host_addr_ret, addr_len, flags);
    }

    bool
    Socket::Listen(int maxconn)
    {
        return listen(_fd, maxconn);
    }

    bool
    Socket::GetName(struct sockaddr *ret_addr, socklen_t *addr_len)
    {
        return getsockname(_fd, ret_addr, addr_len);
    }

    bool
    Socket::GetPeerName(struct sockaddr *remote_host_addr_ret, socklen_t *addr_len)
    {
        return getpeername(_fd, remote_host_addr_ret, addr_len);
    }

    bool
    Socket::Connect(struct sockaddr *connect_to_addr, socklen_t addr_len)
    {
        return connect(_fd, connect_to_addr, addr_len);
    }

    bool
    Socket::Shutdown(int shutdown_type)
    {
        return shutdown(_fd, shutdown_type);
    }

    bool
    Socket::Send(const IOBuff& buff, int flags) const
    {
        return send(_fd, buff.read(-1), buff.len(), flags);
    }

    bool
    Socket::Recv(IOBuff& to, size_t len, int flags)
    {
        to.resize(len);
        return recv(_fd, to.pointer(), len, flags);
    }
    /* bool sendTo(); */
    /* bool recvFrom(); */
    /* bool sendMsg(); */
    /* bool recvMsg(); */

    bool
    Socket::GetOpts(int level, int option, void *value, socklen_t *opt_len)
    {
        return getsockopt(_fd, level, option, value, opt_len);
    }

    bool
    Socket::SetOpts(int level, int option, const int *value, socklen_t opt_len)
    {
        return setsockopt(_fd, level, option, value, opt_len);
    }

    bool Socket::IsOK() { return _fd; }

} /* TCP */ 
