#include "siigix/Net/Socket.hpp"
#include <stdexcept>
#include <stdlib.h>

namespace sgx {
    namespace Net {

        /**********************************************************************
        *                              BaseSocket                               *
        **********************************************************************/

        BaseSocket::BaseSocket(const SocketAddress& addr, int fd)
            : _fd(fd), _addr(addr)
        {
            if (fd == SGX_INVALID_SOCK) {
                throw std::runtime_error(eprintf("BaseSocket::", __func__, ": bad socket: ", strerror(errno)));
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
        BaseSocket::getOption(int level, int option, void *value, socklen_t *opt_len) const
        {
            return (getsockopt(_fd, level, option, value, opt_len) == 0) ? true : false;
        }

        bool
        BaseSocket::getOption(SocketOption& opt) const
        {
            return (getsockopt(_fd, opt.level, opt.optName, &opt.optValue, &opt.optLen) == 0) ? true : false;
        }

        bool
        BaseSocket::setOption(int level, int option, const int *value, socklen_t opt_len)
        {
            int rc = setsockopt(getSocketFD(), level, option, value, opt_len);
            if (rc != 0) {
                throw std::domain_error(eprintf("BaseSocket::", __func__, ": setsockopt: ", strerror(errno)));
            }

            return rc == 0 ? true : false;
        }

        bool
        BaseSocket::setOption(const SocketOption& opt)
        {
            int rc = setsockopt(getSocketFD(), opt.level, opt.optName, &opt.optValue, opt.optLen);
            if (rc != 0) {
                throw std::domain_error(eprintf("BaseSocket::", __func__, ": setsockopt: ", strerror(errno)));
            }

            return rc == 0 ? true : false;
        }

        void
        BaseSocket::setOptions(const SocketOptions& opts)
        {
            for (const SocketOption& opt: opts) {
                setOption(opt);
            }
        }

        void
        BaseSocket::Close()
        {
            if (!isValid()) {
                throw std::logic_error(eprintf("TransSocket::", __func__, ": accept called on a bad socket object (this object was moved)"));
            }

            while (true) {
                int state = ::close(_fd);
                if (state == 0) {
                    break;
                }
                switch(errno)
                {
                    case EBADF: throw std::domain_error(eprintf("BaseSocket::", __func__, ": close: EBADF: ", _fd, " ", strerror(errno)));
                    case EIO:   throw std::runtime_error(eprintf("BaseSocket::", __func__, ": close: EIO:  ", _fd, " ", strerror(errno)));
                    case EINTR:
                    {
                        // TODO: Check for user interrupt flags.
                        // Beyond the scope of this project
                        // so continue normal operations.
                        break;
                    }
                    default:
                        throw std::runtime_error(eprintf("BaseSocket::", __func__, ": close: ???:  ", _fd, " ", strerror(errno)));
                        break;
                }
            }

            _fd = SGX_INVALID_SOCK;
        }

        bool
        BaseSocket::EnableKeepAlive() {
            int flag = 1;

            if (setOption(SOL_SOCKET,  SO_KEEPALIVE,  &flag,              sizeof(flag)) != 0)              { return false; }
            if (setOption(IPPROTO_TCP, TCP_KEEPIDLE,  &_ka_conf.ka_idle,  sizeof(_ka_conf.ka_idle)) != 0)  { return false; }
            if (setOption(IPPROTO_TCP, TCP_KEEPINTVL, &_ka_conf.ka_intvl, sizeof(_ka_conf.ka_intvl)) != 0) { return false; }
            if (setOption(IPPROTO_TCP, TCP_KEEPCNT,   &_ka_conf.ka_cnt,   sizeof(_ka_conf.ka_cnt)) != 0)   { return false; }

            return true;
        }

        void
        BaseSocket::swap(BaseSocket& other) noexcept
        {
            std::swap(_fd, other._fd);
            std::swap(_addr, other._addr);
        }

        BaseSocket::BaseSocket(BaseSocket&& moveobj) noexcept
            : _fd(SGX_INVALID_SOCK)
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

        ConnectSocket::ConnectSocket(const SocketAddress& addr)
            : TransSocket(addr, ::socket(addr.af(), SOCK_STREAM, IPPROTO_TCP))
        {
            if (::connect(getSocketFD(), addr.addr(), addr.length()) == -1) {
                Close();
                throw std::runtime_error(eprintf("ConnectSocket::", __func__, ": connect: ", strerror(errno)));
            }
        }

        ConnectSocket::ConnectSocket(const IPAddress& ip, port_t port)
            : ConnectSocket(SocketAddress(ip, port)) {  }

        /**********************************************************************
        *                            ListenSocket                            *
        **********************************************************************/

        ListenSocket::ListenSocket(const SocketAddress& addr, const SocketOptions& opts, int max_conn)
            : BaseSocket(addr, ::socket(addr.af(), SOCK_STREAM, IPPROTO_TCP))
        {
            setOptions(opts);

            if (::bind(getSocketFD(), addr.addr(), addr.length()) != 0) {
                Close();
                /* std::cerr << addr.toString() << std::endl; */
                /* std::cerr << addr.host().toString() << " port: " << addr.port() << std::endl; */
                throw std::runtime_error(eprintf("ListenSocket::", __func__, ": bind: ", strerror(errno)));
            }

            if (::listen(getSocketFD(), max_conn) != 0)
            {
                Close();
                throw std::runtime_error(eprintf("ListenSocket::", __func__, ": listen: ", strerror(errno)));
            }
        }

        ListenSocket::ListenSocket(const IPAddress& ip, port_t port, const SocketOptions& opts, int max_conn) :
            ListenSocket(SocketAddress(ip, port), opts, max_conn) {  }

        TransSocket
        ListenSocket::Accept(int flags)
        {
            if (getSocketFD() == SGX_INVALID_SOCK) {
                throw std::logic_error(eprintf("ListenSocket::", __func__, ": accept called on a bad socket object (this object was moved)"));
            }

            struct sockaddr_storage ss;
            sgx_socklen_t addr_size = sizeof ss;
            int newSocket = ::accept4(getSocketFD(), (struct sockaddr*)&ss, &addr_size, flags);
            if (newSocket == SGX_INVALID_SOCK) {
                throw std::runtime_error(eprintf("ListenSocket:", __func__, ": accept: ", strerror(errno)));
            }

            return TransSocket(SocketAddress((struct sockaddr*)&ss, addr_size), newSocket);
        }

        /**********************************************************************
        *                             TransSocket                             *
        **********************************************************************/

        TransSocket::TransSocket(const SocketAddress& addr, int fd) :
            BaseSocket(addr, fd)
        { }

        void
        TransSocket::sendBytes(const char* data, std::size_t size)
        {
            std::size_t dataWritten = 0;

            while(dataWritten < size)
            {
                std::size_t put = send(getSocketFD(), data + dataWritten, size - dataWritten, 0);
                put = checkReciveState(put);
                dataWritten += put;
            }
            return;
        }

        void
        TransSocket::sendCloseMessage()
        {
            if (::shutdown(getSocketFD(), SHUT_WR) != 0) {
                throw std::domain_error(eprintf("TCP::Protocol::", __func__, ": shutdown: critical error: ", strerror(errno)));
            }
        }

        int
        TransSocket::checkReciveState(int get)
        {
            if (get == static_cast<std::size_t>(-1)) {
                switch(errno) {
                    case EBADF:
                    case EFAULT:
                    case EINVAL:
                    case ENXIO:
                    {
                        // Fatal error. Programming bug
                        throw std::domain_error(eprintf("TransSocket::", __func__, ": read: critical error: ", strerror(errno)));
                    }
                    case EIO:
                    case ENOBUFS:
                    case ENOMEM:
                    {
                       // Resource acquisition failure or device error
                        throw std::runtime_error(eprintf("TransSocket::", __func__, ": read: resource failure: ", strerror(errno)));
                    }
                    case EINTR:
                        // TODO: Check for user interrupt flags.
                        //       Beyond the scope of this project
                        //       so continue normal operations.
                    case ETIMEDOUT:
                    case EAGAIN:
                    {
                        // Temporary error.
                        // Simply retry the read.
                        /* continue; */
                    }
                    case ECONNRESET:
                    case ENOTCONN:
                    {
                        // Connection broken.
                        // Return the data we have available and exit
                        // as if the connection was closed correctly.
                        get = 0;
                        break;
                    }
                    default:
                    {
                        throw std::runtime_error(eprintf("TransSocket::", __func__, ": read: returned -1: ", strerror(errno)));
                    }
                }
            }

            return get;
        }

        int
        TransSocket::checkSendState(int put)
        {
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
                        throw std::domain_error(eprintf("TransSocket::", __func__, ": write: critical error: ", strerror(errno)));
                    }
                    case EDQUOT:
                    case EFBIG:
                    case EIO:
                    case ENETDOWN:
                    case ENETUNREACH:
                    case ENOSPC:
                    {
                        // Resource acquisition failure or device error
                        throw std::runtime_error(eprintf("TransSocket::", __func__, ": write: resource failure: ", strerror(errno)));
                    }
                    case EINTR:
                            // TODO: Check for user interrupt flags.
                            //       Beyond the scope of this project
                            //       so continue normal operations.
                    case EAGAIN:
                    {
                        // Temporary error.
                        // Simply retry the read.
                        /* continue; */
                    }
                    default:
                    {
                        throw std::runtime_error(eprintf("TransSocket::", __func__, ": write: returned -1: ", strerror(errno)));
                    }
                }
            }
            return put;
        }
    } /* Net */ 
}
