#include <Socket.hpp>

namespace siigix {
namespace INet {

std::map<std::string, in_addr_t>
getLocalIP()
{
    std::map<std::string, in_addr_t> addresses;
    char buff[64];
    hostent *sh;

    if (!gethostname(buff, sizeof(buff))) {
        if ( (sh = gethostbyname(buff)) ) {
            int interface_n = 0;
            while (sh->h_addr_list[interface_n]) {
                struct sockaddr_in addr;
                memcpy(&addr.sin_addr, sh->h_addr_list[interface_n], sh->h_length);
                addresses[sh->h_aliases[interface_n]] = addr.sin_addr.s_addr;
                interface_n++;
            }
        }
    }
    //cleanup TODO

    return addresses;
}

bool
operator==(struct sockaddr_in a, struct sockaddr_in b)
{
    if (
            a.sin_addr.s_addr == b.sin_addr.s_addr &&
            /* a.sin_family == b.sin_family && */
            a.sin_port == b.sin_port
        )
    {
        return true;
    }
    return false;
}

bool
operator==(struct sockaddr_in6 a, struct sockaddr_in6 b)
{
    if (
            a.sin6_flowinfo == b.sin6_flowinfo &&
            /* !strcmp(a.sin6_addr.s6_addr, b.sin6_addr.s6_addr) && */
            /* a.sin6_family == b.sin6_family && //is need(alvays AF_INET6)? */
            a.sin6_port == b.sin6_port &&
            a.sin6_scope_id == b.sin6_scope_id
        )
    {
        for (int i = 0; i < 16; i++) { //use sizeof?
            if (a.sin6_addr.s6_addr[i] != b.sin6_addr.s6_addr[i]) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool
operator==(const sockaddr_in_any a, const sockaddr_in_any b)
{
    if (a.is_v6() == b.is_v6()) {
        if (a.is_v6() && a.ip6() == b.ip6()) {
            return true;
        } else if (a.ip4() == b.ip4()) {
            return true;
        }
    }
    return false;
}

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

Socket::Status Socket::IsUP() {
    return _status;
}

}
}
