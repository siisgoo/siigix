#ifndef SOCKETADDRESS_HPP_SNZTXJLY
#define SOCKETADDRESS_HPP_SNZTXJLY

#include "siigix/General/eprintf.hpp"
#include "siigix/Net/IPAddress.hpp"
#include "siigix/Net/INetDefs.hpp"

#include <memory>
#include <iostream>

namespace sgx {
    namespace Net {
        typedef __uint16_t port_t;

        class SocketAddress {
            public:
                SocketAddress();
                explicit SocketAddress(AddressFamily);
                SocketAddress(const IPAddress&, port_t);
                explicit SocketAddress(port_t);
                SocketAddress(AddressFamily, port_t);
                SocketAddress(const std::string& host, port_t);
                SocketAddress(AddressFamily, const std::string& host, port_t);
                SocketAddress(const std::string& host, const std::string& port);
                SocketAddress(AddressFamily, const std::string& host, const std::string& port);
                explicit SocketAddress(const std::string& HostPort);
                SocketAddress(const struct sockaddr* addr, sgx_socklen_t len);

                IPAddress host() const;
                port_t port() const;
                sgx_socklen_t length() const;
                const struct sockaddr* addr() const;
                int af() const;
                AddressFamily family() const;
                std::string toString() const;
                #ifdef __linux__
                std::string path() const;
                #endif

                SocketAddress(const SocketAddress& other);
                SocketAddress& operator = (const SocketAddress& other);

                bool operator == (const SocketAddress& socketAddress) const;
                bool operator != (const SocketAddress& socketAddress) const;

                virtual ~SocketAddress ();

            protected:
                void init(const IPAddress& hostAddress, port_t);
                void init(const std::string& hostAddress, port_t);
                void init(AddressFamily, const std::string& hostAddress, port_t);
                void init(AddressFamily, const std::string& address);
                void init(const std::string& hostAndPort);
                port_t resolveService(const std::string& service);

            private:
                void initIPv4();
                void initIPv4(const struct sockaddr_in*);
                void initIPv4(const IPAddress& addr, port_t);
                void initIPv6();
                void initIPv6(const struct sockaddr_in6*);
                void initIPv6(const IPAddress& addr, port_t);

                #ifdef __linux__
                    void initLocal(const struct sockaddr_un*);
                    void initLocal(const std::string& sockPath);
                    struct sockaddr_un _uaddr;
                #endif

                port_t _port;
                AddressFamily _family;
                struct sockaddr_in  _saddr4;
                struct sockaddr_in6 _saddr6;
        };

    } /*  Net */ 
} /* sgx  */ 

#endif /* end of include guard: SOCKETADDRESS_HPP_SNZTXJLY */
