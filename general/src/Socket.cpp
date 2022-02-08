#include <general/Socket.hpp>
#include <stdexcept>

namespace siigix {
    //IS OK??????????????????????? TODO
    int getLocalIP(std::map<std::string, struct sockaddr> &ip_map)
    {
        int interface_n = 0;
        char buff[64];
        hostent *sh;
        struct sockaddr_storage ss;
        struct sockaddr cur_addr;

        if (!gethostname(buff, sizeof(buff))) {
            if ( (sh = gethostbyname(buff)) ) {
                while (sh->h_addr_list[interface_n]) {
                    if (sh->h_addrtype == AF_INET) {
                        /* memcpy(&.sin_addr, sh->h_addr_list[interface_n], sh->h_length); */
                    } else if (sh->h_addrtype == AF_INET6) {
                        /* memcpy(&.sin6_addr, sh->h_addr_list[interface_n], sh->h_length); */
                    } else {
                        //very big error o_O
                    }
                    ip_map[sh->h_aliases[interface_n]] = cur_addr;
                    interface_n++;
                }
            }
        }
        //cleanup TODO

        return interface_n;
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

    /**********************************************************************
    *                              BaseSocket                               *
    **********************************************************************/

    BaseSocket::BaseSocket(int fd)
        : _fd(fd)
    {
        if (fd == INVALID_SOCK) {
            throw std::runtime_error(buildErrorMessage("BaseSocket::", __func__, ": bad socket: ", strerror(errno)));
        }
    }

    BaseSocket::~BaseSocket()
    {
        if (!isValid()) {
            return;
        }

        try {
            Close();
        } catch (std::exception& except) {
            //TODO ADD LOG!
            std::cerr << except.what();
        }
    }

    bool
    BaseSocket::GetOpts(int level, int option, void *value, socklen_t *opt_len) const
    {
        return (getsockopt(_fd, level, option, value, opt_len) == 0) ? true : false;
    }

    bool
    BaseSocket::SetOpts(int level, int option, const int *value, socklen_t opt_len)
    {
        int rc = setsockopt(_fd, level, option, value, opt_len);
        switch (rc)
        {
            case EBADF:
                throw std::domain_error(buildErrorMessage("BaseSocket::", __func__, ": setsockopt: EBADF: ", _fd, " ", strerror(rc)));
                break;

            case EDOM:
                throw std::domain_error(buildErrorMessage("BaseSocket::", __func__, ": setsockopt: EDOM: ", _fd, " ", strerror(rc)));
                break;

            case ENOTSOCK:
            case EINVAL:
                throw std::domain_error(buildErrorMessage("BaseSocket::", __func__, ": setsockopt: critical error: ", strerror(rc)));
                break;

            case EISCONN:
                throw std::runtime_error(buildErrorMessage("BaseSocket::", __func__, ": setsockopt: EISCONN: ", strerror(rc)));
                break;

            case ENOPROTOOPT:
                throw std::runtime_error(buildErrorMessage("BaseSocket::", __func__, ": setsockopt: EISCONN: ", strerror(rc)));
                break;
                throw std::runtime_error(buildErrorMessage("BaseSocket::", __func__, ": setsockopt: : ", strerror(rc)));
                break;

            case ENOMEM:
            case ENOBUFS:
                throw std::runtime_error(buildErrorMessage("BaseSocket::", __func__, ": setsockopt: not anought machine resources: ", strerror(rc)));
                break;

            default:
                throw std::runtime_error(buildErrorMessage("BaseSocket::", __func__, ": SetOpts: ???:  ", _fd, " ", strerror(rc)));
                break;
        }

        return rc == 0 ? true : false;
    }

    void
    BaseSocket::Close()
    {
        if (!isValid()) {
            throw std::logic_error(buildErrorMessage("DataSocket::", __func__, ": accept called on a bad socket object (this object was moved)"));
        }

        while (true) {
            int state = ::close(_fd);
            if (state == 0) {
                break;
            }
            switch(errno)
            {
                case EBADF: throw std::domain_error(buildErrorMessage("BaseSocket::", __func__, ": close: EBADF: ", _fd, " ", strerror(errno)));
                case EIO:   throw std::runtime_error(buildErrorMessage("BaseSocket::", __func__, ": close: EIO:  ", _fd, " ", strerror(errno)));
                case EINTR:
                {
                    // TODO: Check for user interrupt flags.
                    // Beyond the scope of this project
                    // so continue normal operations.
                    break;
                }
                default:
                    throw std::runtime_error(buildErrorMessage("BaseSocket::", __func__, ": close: ???:  ", _fd, " ", strerror(errno)));
                    break;
            }
        }

        _fd = INVALID_SOCK;
    }

    bool
    BaseSocket::EnableKeepAlive() {
        int flag = 1;

        if (SetOpts(SOL_SOCKET,  SO_KEEPALIVE,  &flag,              sizeof(flag)) != 0)              { return false; }
        if (SetOpts(IPPROTO_TCP, TCP_KEEPIDLE,  &_ka_conf.ka_idle,  sizeof(_ka_conf.ka_idle)) != 0)  { return false; }
        if (SetOpts(IPPROTO_TCP, TCP_KEEPINTVL, &_ka_conf.ka_intvl, sizeof(_ka_conf.ka_intvl)) != 0) { return false; }
        if (SetOpts(IPPROTO_TCP, TCP_KEEPCNT,   &_ka_conf.ka_cnt,   sizeof(_ka_conf.ka_cnt)) != 0)   { return false; }

        return true;
    }

    void
    BaseSocket::swap(BaseSocket& other) noexcept
    {
        std::swap(_fd, other._fd);
    }

    BaseSocket::BaseSocket(BaseSocket&& moveobj) noexcept
        : _fd(INVALID_SOCK)
    {
        moveobj.swap(*this);
    }

    BaseSocket&
    BaseSocket::operator=(BaseSocket&& moveobj) noexcept
    {
        moveobj.swap(*this);
        return *this;
    }

    /**********************************************************************
    *                           ConnectSocket                            *
    **********************************************************************/

    ConnectSocket::ConnectSocket(std::string ip, port_t port, struct addrinfo hints) :
        DataSocket(::socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol))
    {
        struct sockaddr_in serverAddr{};
        serverAddr.sin_family       = AF_INET;
        serverAddr.sin_port         = htons(port);
        serverAddr.sin_addr.s_addr  = inet_addr(ip.c_str());

        if (::connect(getSocketFD(), (struct sockaddr*)&serverAddr, sizeof(serverAddr)) != 0)
        {
            Close();
            throw std::runtime_error(buildErrorMessage("ConnectSocket::", __func__, ": connect: ", strerror(errno)));
        }
        /* struct addrinfo *addr; */

        /* char szPort[6]; */
        /* sprintf(szPort, "%hu", port); */

        /* if (getaddrinfo(ip.c_str(), szPort, &hints, &addr) == 0) { */
        /*     if (::connect(getSocketFD(), addr->ai_addr, addr->ai_addrlen) == -1) { */
        /*         throw std::runtime_error(buildErrorMessage("ConnectSocket::", __func__, ": connect: ", strerror(errno))); */
        /*     } */
        /* } else { */
        /*     throw std::runtime_error(buildErrorMessage("ConnectScoket::", __func__, ": getaddrinfo: ", strerror(errno))); */
        /* } */

        /* freeaddrinfo(addr); */
    }

    /**********************************************************************
    *                            ListenSocket                            *
    **********************************************************************/

    ListenSocket::ListenSocket(port_t port, int max_conn) :
        BaseSocket(::socket(PF_INET, SOCK_STREAM, 0))
    {
        struct sockaddr_in l_addr;
        bzero((char*)&l_addr, sizeof(l_addr));
        l_addr.sin_family       = AF_INET;
        l_addr.sin_port         = htons(port);
        l_addr.sin_addr.s_addr  = htonl(INADDR_ANY);

        if (::bind(getSocketFD(), (struct sockaddr*)&l_addr, sizeof(l_addr)) != 0) {
            Close();
            throw std::runtime_error(buildErrorMessage("ListenSocket::", __func__, ": bind: ", strerror(errno)));
        }

        if (::listen(getSocketFD(), max_conn) != 0)
        {
            Close();
            throw std::runtime_error(buildErrorMessage("ListenSocket::", __func__, ": listen: ", strerror(errno)));
        }
    }

    DataSocket
    ListenSocket::Accept()
    {
        if (getSocketFD() == INVALID_SOCK) {
            throw std::logic_error(buildErrorMessage("ListenSocket::", __func__, ": accept called on a bad socket object (this object was moved)"));
        }

        struct sockaddr_storage serverStorage;
        socklen_t addr_size = sizeof serverStorage;
        int newSocket = ::accept(getSocketFD(), (struct sockaddr*)&serverStorage, &addr_size);
        if (newSocket == INVALID_SOCK) {
            throw std::runtime_error(buildErrorMessage("ListenSocket:", __func__, ": accept: ", strerror(errno)));
        }
        return DataSocket(newSocket);
    }

    /**********************************************************************
    *                             DataSocket                             *
    **********************************************************************/

    void
    DataSocket::sendMessage(const char* data, size_t size)
    {
        std::size_t dataWritten = 0;

        while(dataWritten < size)
        {
            std::size_t put = write(getSocketFD(), data + dataWritten, size - dataWritten);
            if (put == static_cast<std::size_t>(-1))
            {
                switch(errno)
                {
                    case EINVAL:
                    case EBADF:
                    case ECONNRESET:
                    case ENXIO:
                    case EPIPE:
                    {
                        // Fatal error. Programming bug
                        throw std::domain_error(buildErrorMessage("DataSocket::", __func__, ": write: critical error: ", strerror(errno)));
                    }
                    case EDQUOT:
                    case EFBIG:
                    case EIO:
                    case ENETDOWN:
                    case ENETUNREACH:
                    case ENOSPC:
                    {
                        // Resource acquisition failure or device error
                        throw std::runtime_error(buildErrorMessage("DataSocket::", __func__, ": write: resource failure: ", strerror(errno)));
                    }
                    case EINTR:
                            // TODO: Check for user interrupt flags.
                            //       Beyond the scope of this project
                            //       so continue normal operations.
                    case EAGAIN:
                    {
                        // Temporary error.
                        // Simply retry the read.
                        continue;
                    }
                    default:
                    {
                        throw std::runtime_error(buildErrorMessage("DataSocket::", __func__, ": write: returned -1: ", strerror(errno)));
                    }
                }
            }
            dataWritten += put;
        }
        return;
    }

    void
    DataSocket::sendMessageClose()
    {
        if (::shutdown(getSocketFD(), SHUT_WR) != 0) {
            throw std::domain_error(buildErrorMessage("TCP::Protocol::", __func__, ": shutdown: critical error: ", strerror(errno)));
        }
    }

}
