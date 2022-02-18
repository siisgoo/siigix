#ifndef SOCKET_HPP_LEMCELXN
#define SOCKET_HPP_LEMCELXN

#include "siigix/General/eprintf.hpp"

#include "siigix/Net/INetDefs.hpp"
#include "siigix/Net/SocketAddress.hpp"
#include "siigix/Net/SocketOptions.hpp"

#include <map>
#include <stdexcept>
#include <string>
#include <iostream>
#include <initializer_list>

namespace sgx {
    namespace Net {

        using SocketByteBuf = std::vector<unsigned char>;

        /**********************************************************************
        *                              BaseSocket                               *
        **********************************************************************/

        class BaseSocket {
            public:
                //cannot be two same sockets
                BaseSocket(const BaseSocket& other) = delete;

                void Close();

                BaseSocket(BaseSocket&& move)            noexcept;
                BaseSocket& operator=(BaseSocket&& move) noexcept;
                void swap(BaseSocket& other)             noexcept;

                bool getOption(int level, int option, void *value, socklen_t *opt_len) const;
                bool getOption(SocketOption& opt) const;
                bool setOption(int level, int option, const int *value, socklen_t opt_len);
                bool setOption(const SocketOption& opt);
                void setOptions(const SocketOptions& opt);

                bool EnableKeepAlive();
                void SetKeepAlive(KeepAliveConfig ka) { _ka_conf = ka; }

                bool isValid() const { return ((_fd != SGX_INVALID_SOCK) ? true : false); };

                virtual const SocketAddress& address() const { return _addr; }

                virtual ~BaseSocket();

            protected:
                BaseSocket(const SocketAddress& addr,int fd);

                int getSocketFD() const { return _fd; }

            private:
                sgx_socket_t    _fd;
                SocketAddress   _addr;
                KeepAliveConfig _ka_conf;
        };

        /**********************************************************************
        *                             TransSocket                             *
        **********************************************************************/

        /* send/recive data to socket */
        class TransSocket : public BaseSocket {
            public:
                TransSocket(const SocketAddress&, int fd);

                template<typename Fn>
                std::size_t reciveBytes(char *data, std::size_t size, Fn scanForEnd = [](std::size_t) { return false; });
                void        sendBytes(const char* data, std::size_t size);
                void        sendCloseMessage();

            private:
                int checkReciveState(int rc);
                int checkSendState(int rc);
        };


        /**********************************************************************
        *                           ConnectSocket                            *
        **********************************************************************/

        /* Connect to remote host
         * Avaible to send/recive data */
        class ConnectSocket : public TransSocket {
            public:
                ConnectSocket(const SocketAddress&);
                ConnectSocket(const IPAddress&, port_t);
        };

        /**********************************************************************
        *                            ListenSocket                            *
        **********************************************************************/

        /* listen for connection */
        class ListenSocket : public BaseSocket {
            public:
                ListenSocket(const SocketAddress&, const SocketOptions& opts, int max_conn = MAX_CONN);
                ListenSocket(const IPAddress&, port_t port, const SocketOptions& opts, int max_conn = MAX_CONN);

                TransSocket Accept(int flags = 0);
        };

        #include "Socket.inl"
    } /* Net */ 

} /* sgx */ 

#endif /* end of include guard: SOCKET_HPP_LEMCELXN */
