#include "siigix/Net/SocketAddress.hpp"
#include "siigix/Net/DNS.hpp"
#include "siigix/Net/HostEntry.hpp"
#include "siigix/Net/INetDefs.hpp"

#include <limits>
#include <stdexcept>
#include <string>

namespace sgx {
    namespace Net {

    struct AFLT {
        bool operator()(const IPAddress& a1, const IPAddress& a2) {
            return a1.af() < a2.af();
        }
    };

        SocketAddress::SocketAddress(const SocketAddress& other)
        {
            if (other.family() == IPv4)
                initIPv4(
                    reinterpret_cast<const sockaddr_in*>(other.addr()));
            else if (other.family() == IPv6)
                initIPv6(reinterpret_cast<const sockaddr_in6*>(
                    other.addr()));
            #if defined(__linux__)
            else if (other.family() == UNIX_LOCAL)
                initLocal(
                    reinterpret_cast<const sockaddr_un*>(other.addr()));
            #endif
        }

        SocketAddress::~SocketAddress() {  }

        SocketAddress::SocketAddress()
        {
            initIPv4();
        }


        SocketAddress::SocketAddress(AddressFamily f)
        {
            init(IPAddress(f), 0);
        }

        SocketAddress::SocketAddress(const IPAddress& addr, port_t port)
        {
            init(addr, port);
        }

        SocketAddress::SocketAddress(port_t port)
        {
            init(IPAddress(), port);
        }

        SocketAddress::SocketAddress(AddressFamily f, port_t port)
        {
            init(IPAddress(f), port);
        }

        SocketAddress::SocketAddress(const std::string& host, port_t port)
        {
            init(host, port);
        }

        SocketAddress::SocketAddress(AddressFamily f, const std::string& host, port_t port)
        {
            init(f, host, port);
        }

        SocketAddress::SocketAddress(const std::string& host, const std::string& port)
        {
            init(host, resolveService(port));
        }

        SocketAddress::SocketAddress(AddressFamily f, const std::string& host, const std::string& port)
        {
            init(f, host, resolveService(port));
        }

        SocketAddress::SocketAddress(const std::string& HostPort)
        {
            init(HostPort);
        }

        SocketAddress::SocketAddress(const struct sockaddr* addr, sgx_socklen_t len)
        {
            if (len == sizeof(struct sockaddr_in) &&
                addr->sa_family == AF_INET)
                initIPv4(reinterpret_cast<const struct sockaddr_in*>(addr));
            else if (len == sizeof(struct sockaddr_in6) &&
                     addr->sa_family == AF_INET6)
                initIPv6(reinterpret_cast<const struct sockaddr_in6*>(addr));
            #if defined(__linux__)
            else if (len > 0 && len <= sizeof(struct sockaddr_un) &&
                     addr->sa_family == AF_UNIX)
                initLocal(reinterpret_cast<const sockaddr_un*>(addr));
            #endif
            else
                throw std::runtime_error(eprintf("SocketAddress::", __func__, " Invalid address length or family passed!"));
        }

        IPAddress
        SocketAddress::host() const {
            if (_family == AddressFamily::IPv4) {
                return IPAddress(&_saddr4.sin_addr, sizeof(_saddr4.sin_addr));
            } else if (_family == AddressFamily::IPv6) {
                return IPAddress(&_saddr6.sin6_addr, sizeof(_saddr6.sin6_addr), _saddr6.sin6_scope_id);
            }
            #ifdef __linux__
            else if (_family == AddressFamily::UNIX_LOCAL) {
                throw std::runtime_error(eprintf("SocketAddress::", __func__, "Unix local socket cannot have a Host name"));
            }
            #endif

            throw std::runtime_error(eprintf("SocketAddress::", __func__, "Undefine AddressFamily: ", _family));
        }

        port_t
        SocketAddress::port() const
        {
            if (_family == AddressFamily::IPv4) {
                return _saddr4.sin_port;
            } else if (_family == AddressFamily::IPv6) {
                return _saddr6.sin6_port;
            }
            #ifdef __linux__
            else if (_family == AddressFamily::UNIX_LOCAL) {
                throw std::runtime_error(eprintf("SocketAddress::", __func__, "Unix local socket cannot have a Port"));
            }
            #endif

            throw std::runtime_error(eprintf("SocketAddress::", __func__, "Undefine AddressFamily: ", _family));
        }

        sgx_socklen_t
        SocketAddress::length() const
        {
            if (_family == AddressFamily::IPv4) {
                return sizeof(_saddr4);
            } else if (_family == AddressFamily::IPv6) {
                return sizeof(_saddr6);
            }
            #ifdef __linux__
            else if (_family == AddressFamily::UNIX_LOCAL) {
                return sizeof(_uaddr);
            }
            #endif

            throw std::runtime_error(eprintf("SocketAddress::", __func__, "Undefine AddressFamily: ", _family));
        }

        const struct sockaddr*
        SocketAddress::addr() const
        {
            if (_family == AddressFamily::IPv4) {
                return reinterpret_cast<const struct sockaddr*>(&_saddr4);
            } else if (_family == AddressFamily::IPv6) {
                return reinterpret_cast<const struct sockaddr*>(&_saddr6);
            }
            #ifdef __linux__
            else if (_family == AddressFamily::UNIX_LOCAL) {
                return reinterpret_cast<const struct sockaddr*>(&_uaddr);
            }
            #endif

            throw std::runtime_error(eprintf("SocketAddress::", __func__, "Undefine AddressFamily: ", _family));
        }

        int
        SocketAddress::af() const
        {
            if (_family == AddressFamily::IPv4) {
                return _saddr4.sin_family;
            } else if (_family == AddressFamily::IPv6) {
                return _saddr6.sin6_family;
            }
            #ifdef __linux__
            else if (_family == AddressFamily::UNIX_LOCAL) {
                return _uaddr.sun_family;
            }
            #endif

            throw std::runtime_error(eprintf("SocketAddress::", __func__, "Undefine AddressFamily: ", _family));
        }

        AddressFamily SocketAddress::family() const
        {
            return _family;
        }

        std::string SocketAddress::toString() const
        {
            std::string ipstr;
            if (_family == AddressFamily::IPv4) {
                ipstr.append(host().toString());
                ipstr.append(":");
                ipstr.append( std::to_string(ntohs(port())) );
                return ipstr;
            } else if (_family == AddressFamily::IPv6) {
                ipstr.append("[");
                ipstr.append(host().toString());
                ipstr.append("]");
                ipstr.append(":");
                ipstr.append( std::to_string(ntohs(port())) );
                return ipstr;
            }
            #ifdef __linux__
            else if (_family == AddressFamily::UNIX_LOCAL) {
                return std::string(path());
            }
            #endif

            throw std::runtime_error(eprintf("SocketAddress::", __func__, "Undefine AddressFamily: ", _family));
        }

        #ifdef __linux__
            std::string
            SocketAddress::path() const
            {
                if (_family != AddressFamily::UNIX_LOCAL) {
                    throw std::runtime_error(eprintf("SocketAddress::", __func__, " Cannot get path of not Unix local socket"));
                }

                return _uaddr.sun_path;
            }
        #endif

        /* inits */
        void
        SocketAddress::init(const IPAddress& hostAddress, port_t port)
        {
            if (hostAddress.family() == AddressFamily::IPv4) {
                initIPv4(hostAddress, port);
            } else if (hostAddress.family() == AddressFamily::IPv6) {
                initIPv6(hostAddress, port);
            } else {
                throw std::runtime_error(eprintf("SocketAddress::", __func__, " : unsupportd IP address family: ", hostAddress.family()));
            }
        }

        void
        SocketAddress::init(const std::string& hostAddress, port_t portNumber)
        {
            IPAddress ip;
            if (IPAddress::tryParse(hostAddress, ip))
            {
                init(ip, portNumber);
            }
            else
            {
                HostEntry he = DNS::hostByName(hostAddress);
                HostEntry::AddressList addresses = he.addresses();
                if (addresses.size() > 0)
                {
                    std::stable_sort(addresses.begin(), addresses.end(), AFLT());
                    init(addresses[0], portNumber);
                }
                else {
                    throw std::runtime_error(eprintf("No address found for host", hostAddress));
                }
            }
        }

        void
        SocketAddress::init(AddressFamily fam, const std::string& hostAddress, port_t portNumber)
        {
            IPAddress ip;
            if (IPAddress::tryParse(hostAddress, ip)) {
                if (ip.family() != fam)
                    throw std::runtime_error(eprintf("SocketAddress::", __func__, " parsed ip: ", hostAddress, " not in family: ", fam));
                init(ip, portNumber);

            } else {
                HostEntry he = DNS::hostByName(hostAddress);
                HostEntry::AddressList addresses = he.addresses();
                if (addresses.size() > 0) {
                    for (const auto& addr : addresses) {
                        if (addr.family() == fam) {
                            init(addr, portNumber);
                            return;
                        }
                    }
                    throw std::runtime_error(eprintf("SocketAddress::", __func__, " parsed ip: ", hostAddress, " not in family: ", fam));

                } else
                    throw std::runtime_error(eprintf("SocketAddress::", __func__, " No address found for host", hostAddress));
            }
        }

        void
        SocketAddress::init(AddressFamily fam, const std::string& address)
        {
            #ifdef __linux__
                if (fam == UNIX_LOCAL) {
                    initLocal(address);
                } else
            #endif
            {
                std::string host;
                std::string port;
                std::string::const_iterator it = address.begin();
                std::string::const_iterator end = address.end();

                if (*it == '[') {
                    ++it;
                    while (it != end && *it != ']') host += *it++;
                    if (it == end) {
                        throw std::runtime_error(eprintf("SocketAddress::", __func__, " Malformed IPv6 address: ", address.c_str()));
                    }
                    ++it;

                } else {
                    while (it != end && *it != ':') host += *it++;
                }
                if (it != end && *it == ':') {
                    ++it;
                    while (it != end) port += *it++;

                } else {
                    throw std::runtime_error(eprintf("SocketAddress::", __func__, " Missing port number"));
                }
                init(fam, host, resolveService(port));
            }
        }

        void
        SocketAddress::init(const std::string& hostAndPort)
        {
            std::string host;
            std::string port;
            std::string::const_iterator it = hostAndPort.begin();
            std::string::const_iterator end = hostAndPort.end();

            #ifdef __linux__
                if (*it == '/') {
                    initLocal(hostAndPort);
                    return;
                }
            #endif
            if (*it == '[') {
                ++it;
                while (it != end && *it != ']') host += *it++;
                if (it == end) {
                    throw std::runtime_error(eprintf("SocketAddress::", __func__, " Malformed IPv6 address: ", hostAndPort.c_str()));
                }
                ++it;

            } else {
                while (it != end && *it != ':') host += *it++;
            }
            if (it != end && *it == ':') {
                ++it;
                while (it != end) port += *it++;

            } else {
                throw std::runtime_error(eprintf("SocketAddress::", __func__, " Missing port number"));
            }
            init(host, resolveService(port));
        }

        port_t
        SocketAddress::resolveService(const std::string& service)
        {
            unsigned port;
            /* if (NumberParser::tryParseUnsigned(service, port) && port <= 0xFFFF) { */
            /*     return (uint16)port; */

            /* } else { */
                struct servent* se = getservbyname(service.c_str(), NULL);
                if (se)
                    return ntohs(se->s_port);
                else
                    throw std::runtime_error(eprintf("SocketAddress::", __func__, " cannot resolve service: ", service.c_str()));
            /* } */
        }

        SocketAddress&
        SocketAddress::operator = (const SocketAddress& other)
        {
            if (&other != this) {
                if (other.family() == IPv4)
                    initIPv4(reinterpret_cast<const sockaddr_in*>(
                        other.addr()));
                else if (other.family() == IPv6)
                    initIPv6(reinterpret_cast<const sockaddr_in6*>(
                        other.addr()));
                #if defined(__linux__)
                else if (other.family() == UNIX_LOCAL)
                    initLocal(reinterpret_cast<const sockaddr_un*>(
                        other.addr()));
                #endif
            }
            return *this;
        }

        bool
        SocketAddress::operator == (const SocketAddress& socketAddress) const
        {
            #if defined(__linux__)
            if (family() == UNIX_LOCAL) {
                return toString() == socketAddress.toString();
            } else
            #endif
                return host() == socketAddress.host() &&
                       port() == socketAddress.port();
        }

        bool
        SocketAddress::operator!=(const SocketAddress& addr) const
        {
            return !(*this == addr);
        }

        /* Inits */
        void SocketAddress::initIPv4()
        {
            _family = AddressFamily::IPv4;
            std::memset(&_saddr4, 0, sizeof(_saddr4));
            _saddr4.sin_family = AF_INET;
        }

        void SocketAddress::initIPv4(const struct sockaddr_in* addr)
        {
            _family = AddressFamily::IPv4;
            std::memcpy(&_saddr4, addr, sizeof(_saddr4));
        }

        void SocketAddress::initIPv4(const IPAddress& addr, port_t port)
        {
            _family = AddressFamily::IPv4;
            std::memset(&_saddr4, 0, sizeof(_saddr4));
            _saddr4.sin_family = AF_INET;
            std::memcpy(&_saddr4.sin_addr, addr.addr(), addr.length());
            /* sgx_set_sin_len(&_saddr4); */
            _saddr4.sin_port = ByteOrder::toNetwork(port);
        }

        void SocketAddress::initIPv6()
        {
            _family = AddressFamily::IPv6;
            std::memset(&_saddr6, 0, sizeof(_saddr6));
            _saddr6.sin6_family = AF_INET6;
        }

        void SocketAddress::initIPv6(const struct sockaddr_in6* addr)
        {
            _family = AddressFamily::IPv6;
            std::memcpy(&_saddr6, addr, sizeof(_saddr6));
        }

        void SocketAddress::initIPv6(const IPAddress& addr, port_t port)
        {
            _family = AddressFamily::IPv6;
            _saddr6.sin6_family = AF_INET6;
            /* sgx_set_sin6_len(&_saddr6); */
            std::memcpy(&_saddr6.sin6_addr, addr.addr(), sizeof(_saddr6.sin6_addr));
            _saddr6.sin6_port = ByteOrder::toNetwork(port);
        }

        #ifdef __linux__
            void SocketAddress::initLocal(const struct sockaddr_un* addr)
            {
                _family = AddressFamily::UNIX_LOCAL;
                std::memcpy(&_uaddr, addr, sizeof(_uaddr));
            }

            void SocketAddress::initLocal(const std::string& path)
            {
                _family = AddressFamily::UNIX_LOCAL;
                /* sgx_set_sun_len(&_uaddr, path.length() + sizeof(struct sockaddr_un) - sizeof(_uaddr.sun_path) + 1); */
                _uaddr.sun_family = AF_UNIX;
                std::strcpy(_uaddr.sun_path, path.c_str());
            }
        #endif

    } /* Net */ 
} /* sgx  */ 
