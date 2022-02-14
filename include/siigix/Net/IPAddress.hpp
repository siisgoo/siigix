#ifndef IPADDRESS_HPP_BPKM2G96
#define IPADDRESS_HPP_BPKM2G96

#include <string>
#include <cstring>

#include <siigix/General/Formatter.hpp>
#include <siigix/General/ByteOrder.hpp>
#include <siigix/General/Types.hpp>

#include "INetDefs.hpp"

namespace sgx {
    namespace Net {

        static const char * IPV4_TEMPLATE = "(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\".\"){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]))";
        static const char * IPV6_TEMPLATE = "(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))";

        class IPAddress {
            public:
                IPAddress(); /*wildcard ipv4*/
                IPAddress(AddressFamily family); /*wildcard*/
                IPAddress(unsigned prefix, AddressFamily family); /*prefixed wildcard*/
                IPAddress(const std::string& addr); /*select family based on format*/
                IPAddress(const std::string& addr, AddressFamily family);
                IPAddress(const void *addr, sgx_socklen_t addr_len); /* pass in_addr or in6_addr */
                IPAddress(const void *addr, sgx_socklen_t addr_len, sgx::uint32 scope);
                IPAddress(const struct sockaddr& sockaddr);
                /* #if defined(_WIN32) */
                /* IPAddress(const SOCKET_ADDRESS& socket_address); */
                /* #endif */

                std::string toString() const; /* transform to std::string */
                void mask(const IPAddress& mask); /* mask ip (addr & mask) IPv4 ONLY! */
                void mask(const IPAddress* mask, const IPAddress* set); /* (addr & mask | set & mask) IPv4 ONLY! */
                unsigned      prefixLength() const;

                /* Getters */
                AddressFamily family() const; /* return IPv4, IPv6 or UNIX_LOCAL on linux */
                int           af() const; /* return AF_INET, AF_INET6 */
                sgx::uint32   scope() const;
                sgx_socklen_t length() const; /* return length of sockaddr ptr */
                const void*   addr() const; /* return sockaddr ptr */

                static IPAddress parse(const std::string& addr); /* convert addr toIPAdress */
                static bool      tryParse(const std::string& addr, IPAddress& result); /* convert addr toIPAdress return true on success */
                static bool      tryDetermineFamily(const std::string& str, AddressFamily& ret);
                static IPAddress wildcard(AddressFamily family = AddressFamily::IPv4); /* create wildcard IPAddress */
                static IPAddress broadcast(); /* create broadcast IPAddress */

                /* cmp address with Reserved special addresses */
                bool isWildcard() const;
                bool isBroadcast() const;
                bool isLoopback() const;
                bool isMulticast() const;
                bool isUnicast() const;
                bool isLinkLocal() const;
                bool isSiteLocal() const;
                bool isIPv4Compatible() const;
                bool isIPv4Mapped() const;
                bool isWellKnownMC() const;
                bool isNodeLocalMC() const;
                bool isLinkLocalMC() const;
                bool isSiteLocalMC() const;
                bool isOrgLocalMC() const;
                bool isGlobalMC() const;

                IPAddress(const IPAddress& other); /* do copy */
                IPAddress& operator = (const IPAddress& addr); /* do copy = */

                bool operator == (const IPAddress& addr) const;
                bool operator != (const IPAddress& addr) const;
                bool operator <  (const IPAddress& addr) const;
                bool operator >  (const IPAddress& addr) const;
                bool operator <= (const IPAddress& addr) const;
                bool operator >= (const IPAddress& addr) const;
                /* IPAddress operator & (const IPAddress& addr) const; */
                /* IPAddress operator | (const IPAddress& addr) const; */
                /* IPAddress operator ^ (const IPAddress& addr) const; */
                /* IPAddress operator ~ () const; */

                enum { MAX_ADDRESS_LENGTH = sizeof(struct in6_addr), };

                virtual ~IPAddress();
            private:
                static IPAddress parseIPv6(const std::string& addr); /* convert addr toIPAdress */
                static IPAddress parseIPv4(const std::string& addr); /* convert addr toIPAdress */

                void initIPv4();
                void initIPv4(const void* hostAddr);
                void initIPv4(unsigned prefix);
                void initIPv6();
                void initIPv6(const void* hostAddr);
                void initIPv6(const void* hostAddr, sgx::uint32 scope);
                void initIPv6(unsigned prefix);

                bool _isIPv6; //change to AddressFamily
                struct in_addr  _addrIPv4;
                struct in6_addr _addrIPv6;
                unsigned        _scope;
        };


        inline AddressFamily IPAddress::family() const       { return _isIPv6 ? AddressFamily::IPv6 : AddressFamily::IPv4; }
        inline int           IPAddress::af() const           { return _isIPv6 ? AF_INET6 : AF_INET; }
        inline sgx::uint32   IPAddress::scope() const        { return _scope; }
        inline sgx_socklen_t IPAddress::length() const       { return _isIPv6 ? sizeof(_addrIPv6) : sizeof(_addrIPv4); }
        inline const void*   IPAddress::addr() const         { if (_isIPv6) return &_addrIPv6; else return &_addrIPv4; }

    } /* Net */ 
} /* sgx  */ 

#endif /* end of include guard: IPADDRESS_HPP_BPKM2G96 */
