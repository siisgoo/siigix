#include <IPAddress.hpp>

#include <eprintf.hpp>

#include <regex>
#include <cstring>
#include <assert.h>

namespace sgx {
    namespace Net {

        template <typename T>
        unsigned maskBits(T val, unsigned size)
        {
            unsigned count = 0;
            if (val) {
                val = (val ^ (val - 1)) >> 1;
                for (count = 0; val; ++count) val >>= 1;
            }
            else {
                count = size;
            }

            return size - count;
        }

        IPAddress::~IPAddress() {  }

        IPAddress::IPAddress(const IPAddress& other)
        {
            if (other.family() == AddressFamily::IPv6) {
                initIPv6(other.addr(), other.scope());
            } else if (other.family() == AddressFamily::IPv4) {
                initIPv4();
            } else {
                throw std::runtime_error(eprintf("IPAddress::", __func__, ": invalid AddressFamily passed: ", other.family()));
            }
        }

        IPAddress::IPAddress()
        {
            initIPv4();
        }

        IPAddress::IPAddress(const void* addr, sgx_socklen_t len)
        {
            if (len == sizeof(struct in_addr)) {
                initIPv4(addr);
            } else if (len == sizeof(struct in6_addr)) {
                initIPv6(addr);
            } else {
                throw std::runtime_error(eprintf("IPAddress::", __func__, ": invalid address passed: ", addr));
            }
        }

        IPAddress::IPAddress(const void* addr, sgx_socklen_t len, sgx::uint32 scope)
        {
            if (len == sizeof(struct in_addr)) {
                initIPv4(addr);
            } else if (len == sizeof(struct in6_addr)) {
                initIPv6(addr, scope);
            } else {
                throw std::runtime_error(eprintf("IPAddress::", __func__, ": invalid addr passed: ", addr));
            }
        }

        IPAddress::IPAddress(AddressFamily family)
        {
            if (family == AddressFamily::IPv4) {
                initIPv4();
            } else if (family == AddressFamily::IPv6) {
                initIPv6();
            } else {
                throw std::runtime_error(eprintf("IPAddress::", __func__, ": invalid AddressFamily passed: ", family));
            }
        }

        IPAddress::IPAddress(unsigned prefix, AddressFamily family)
        {
            if (family == AddressFamily::IPv4)
            {
                if (prefix <= 32) {
                    initIPv4(prefix);
                } else {
                    throw std::runtime_error(eprintf("IPAddress::", __func__, ": invalid prefix size passed: ", prefix, " IPv4 prefix must be in range 0-32"));
                }
            }
            else if (family == AddressFamily::IPv6)
            {
                if (prefix <= 128) {
                    initIPv6(prefix);
                } else {
                    throw std::runtime_error(eprintf("IPAddress::", __func__, ": invalid prefix size passed: ", prefix, " IPv6 prefix must be in range 0-128"));
                }
            }
            else
            {
                throw std::runtime_error(eprintf("IPAddress::", __func__, ": invalid AddressFamily passed: ", family));
            }
        }

        /* ipv4, ipv6 or domain name */
        IPAddress::IPAddress(const std::string& addr)
        {
            if (addr == "0.0.0.0") {
                initIPv4();
                return;
            }

            IPAddress addr4 = parseIPv4(addr);
            if (addr4 != IPAddress(AddressFamily::IPv4)) {
                initIPv4(addr4.addr());
                return;
            }

            if (addr == "::") {
                initIPv6();
                return;
            }

            IPAddress addr6 = parseIPv6(addr);
            if (addr6 != IPAddress(AddressFamily::IPv6)) {
                initIPv6(addr6.addr(), addr6.scope());
                return;
            }

            throw std::runtime_error(eprintf("IPAddress::", __func__, ": invalid address str passed: ", addr.c_str()));
        }

        IPAddress::IPAddress(const std::string& addr, AddressFamily family)
        {
            if (family == AddressFamily::IPv4) {
                IPAddress addr4(IPAddress::parseIPv4(addr));
                initIPv4(addr4.addr());
            } else if (family == AddressFamily::IPv6) {
                IPAddress addr6(IPAddress::parseIPv6(addr));
                initIPv6(addr6.addr(), addr6.scope());
            } else {
                throw std::runtime_error(eprintf("IPAddress::", __func__, ": invalid AddressFamily passed: ", family));
            }
        }

        IPAddress::IPAddress(const struct sockaddr& sockaddr)
        {
            unsigned short family = sockaddr.sa_family;
            if (family == AF_INET) {
                initIPv4(&reinterpret_cast<const struct sockaddr_in*>(&sockaddr)->sin_addr);
            } else if (family == AF_INET6) {
                initIPv6(&reinterpret_cast<const struct sockaddr_in6*>(&sockaddr)->sin6_addr,
                        reinterpret_cast<const struct sockaddr_in6*>(&sockaddr)->sin6_scope_id);
            } else {
                throw std::runtime_error(eprintf("IPAddress::", __func__, ": invalid AF_* passed: ", family));
            }
        }

        #if defined(_WIN32)
            IPAddress::IPAddress(const SOCKET_ADDRESS& socket_address)
            {
                ADDRESS_FAMILY family = socket_address.lpSockaddr->sa_family;
                if (family == AF_INET) {
                    initIPv4(&reinterpret_cast<const struct sockaddr_in*>(socket_address.lpSockaddr)->sin_addr);
                } else if (family == AF_INET6) {
                    initIPv6(&reinterpret_cast<const struct sockaddr_in6*>(socket_address.lpSockaddr)->sin6_addr,
                            reinterpret_cast<const struct sockaddr_in6*>(socket_address.lpSockaddr)->sin6_scope_id);
                } else {
                    throw std::runtime_error(eprintf("IPAddress::", __func__, ": invalid AddressFamily passed: ", family));
                }
            }
        #endif

        /* ---- */

        unsigned
        IPAddress::prefixLength() const
        {
            if (_isIPv6) {
                unsigned bits = 0;
                unsigned bitPos = 128;
                #if defined(__linux__)
                for (int i = 3; i >= 0; --i)
                {
                    unsigned addr = ntohl(_addrIPv6.s6_addr32[i]);
                    if ((bits = maskBits(addr, 32))) return (bitPos - (32 - bits));
                    bitPos -= 32;
                }
                return 0;
                #elif defined(_WIN32_)
                for (int i = 7; i >= 0; --i)
                {
                    unsigned short addr = ByteOrder::toHost(_addr.s6_addr16[i]);
                    if ((bits = maskBits(addr, 16))) return (bitPos - (16 - bits));
                    bitPos -= 16;
                }
                return 0;
                #endif
            } else {
                return maskBits(ntohl(_addrIPv4.s_addr), 32);
            }
        }

        void IPAddress::mask(const IPAddress& mask)
        {
            if (family() == AddressFamily::IPv4 && mask.family() == AddressFamily::IPv4) {
                assert(mask.af() == AF_INET);
                _addrIPv4.s_addr &= mask._addrIPv4.s_addr;
            }
        }

        void IPAddress::mask(const IPAddress* pMask, const IPAddress* pSet)
        {
            if (family() == AddressFamily::IPv4 && pMask->family() == AddressFamily::IPv4 && pSet->family() == AddressFamily::IPv4) {
                assert(pMask->af() == AF_INET && pSet->af() == AF_INET);
                _addrIPv4.s_addr &= static_cast<const IPAddress*>(pMask)->_addrIPv4.s_addr;
                _addrIPv4.s_addr |= static_cast<const IPAddress*>(pSet)->_addrIPv4.s_addr & ~static_cast<const IPAddress*>(pMask)->_addrIPv4.s_addr;
            } else {
                throw std::runtime_error(eprintf("BaseSocket::", __func__, ": connot extract prefix from IPv6"));
            }
        }

        std::string
        IPAddress::toString() const
        {
            std::string ipstr;

            if (_isIPv6) /* v6 */
            {
                const sgx::uint16* words = reinterpret_cast<const uint16*>(&_addrIPv6);
                if ((isIPv4Compatible() && !isLoopback()) || isIPv4Mapped())
                {
                    ipstr.reserve(24);
                    if (words[5] == 0) {
                        ipstr.append("::");
                    } else {
                        ipstr.append("::ffff:");
                    }
                    const uint8* bytes = reinterpret_cast<const uint8*>(&_addrIPv6);
                    if (bytes[12] != 0) /* only 0.0.0.0 can start with zero */
                    {
                        ipstr.append(std::to_string(bytes[12]));
                        ipstr.append(".");
                        ipstr.append(std::to_string(bytes[13]));
                        ipstr.append(".");
                        ipstr.append(std::to_string(bytes[14]));
                        ipstr.append(".");
                        ipstr.append(std::to_string(bytes[15]));
                    }
                    return ipstr;
                }
                else
                {
                    ipstr.reserve(64);
                    bool zeroSequence = false;
                    int i = 0;
                    while (i < 8)
                    {
                        if (!zeroSequence && words[i] == 0)
                        {
                            int zi = i;
                            while (zi < 8 && words[zi] == 0) ++zi;
                            if (zi > i + 1)
                            {
                                i = zi;
                                ipstr.append(":");
                                zeroSequence = true;
                            }
                        }
                        if (i > 0) ipstr.append(":");
                        if (i < 8) ipstr.append( Formatter::toHex( ByteOrder::toHost(words[i++]) ) );
                    }
                    if (_scope > 0)
                    {
                        ipstr.append("%");
                        #if defined(_WIN32)
                            ipstr.append(std::to_string(_scope));
                        #else
                            char buffer[IFNAMSIZ];
                            if (::if_indextoname(_scope, buffer)) {
                                ipstr.append(buffer);
                            } else {
                                ipstr.append(std::to_string(_scope));
                            }
                        #endif
                    }
                    return Formatter::toLower(ipstr);
                }
            }
            else /* v4 */
            {
                const sgx::uint8* bytes = reinterpret_cast<const sgx::uint8*>(&_addrIPv4.s_addr);
                ipstr.reserve(16);
                ipstr.append(std::to_string(bytes[0]));
                ipstr.append(".");
                ipstr.append(std::to_string(bytes[1]));
                ipstr.append(".");
                ipstr.append(std::to_string(bytes[2]));
                ipstr.append(".");
                ipstr.append(std::to_string(bytes[3]));

            }

            return ipstr;
        }

        IPAddress
        IPAddress::parseIPv6(const std::string& addr)
        {
            if (addr.empty()) {
                return IPAddress();
            }

            #if defined(_WIN32)
                struct addrinfo* pAI;
                struct addrinfo hints;
                std::memset(&hints, 0, sizeof(hints));
                hints.ai_flags = AI_NUMERICHOST;
                int rc = getaddrinfo(addr.c_str(), NULL, &hints, &pAI);
                if (rc == 0)
                {
                    IPv6AddressImpl result = IPAddress(&reinterpret_cast<struct sockaddr_in6*>(pAI->ai_addr)->sin6_addr,
                            static_cast<int>(reinterpret_cast<struct sockaddr_in6*>(pAI->ai_addr)->sin6_scope_id));
                    freeaddrinfo(pAI);
                    return result;
                }
                else return IPAddress();
            #else
                struct in6_addr ia;
                std::string::size_type pos = addr.find('%');
                if (std::string::npos != pos)
                {
                    std::string::size_type start = ('[' == addr[0]) ? 1 : 0;
                    std::string unscopedAddr(addr, start, pos - start);
                    std::string scope(addr, pos + 1, addr.size() - start - pos);
                    uint32 scopeId(0);
                    if (!(scopeId = if_nametoindex(scope.c_str())))
                        return IPAddress();
                    if (inet_pton(AF_INET6, unscopedAddr.c_str(), &ia) == 1)
                        return IPAddress(&ia, scopeId);
                    else
                        return IPAddress();
                }
                else
                {
                    if (inet_pton(AF_INET6, addr.c_str(), &ia) == 1) {
                        return IPAddress(&ia, sizeof(ia));
                    }
                    else {
                        return IPAddress();
                    }
                }
            #endif
        }

        IPAddress
        IPAddress::parseIPv4(const std::string& addr)
        {
            if (addr.empty()) return IPAddress();
            #if defined(_WIN32) 
                struct in_addr ia;
                ia.s_addr = inet_addr(addr.c_str());
                if (ia.s_addr == INADDR_NONE && addr != "255.255.255.255")
                    return IPAddress();
                else
                    return IPAddress(&ia);
            #else
                #if __GNUC__ < 3 || defined(POCO_VXWORKS)
                    struct in_addr ia;
                    ia.s_addr = inet_addr(const_cast<char*>(addr.c_str()));
                    if (ia.s_addr == INADDR_NONE && addr != "255.255.255.255") {
                        return IPAddress();
                    } else {
                        return IPAddress(&ia);
                    }
                #else
                    struct in_addr ia;
                    if (inet_aton(addr.c_str(), &ia))
                        return IPAddress(&ia, sizeof(ia));
                    else
                        return IPAddress();
                #endif
            #endif
        }

        /* STATIC MEMBERS */
        IPAddress
        IPAddress::parse(const std::string& addr)
        {
            return IPAddress(addr);
        }

        bool
        IPAddress::tryParse(const std::string& addr, IPAddress& result) //TODO
        {
            AddressFamily fml;
            if (! IPAddress::tryDetermineFamily(addr, fml)) {
                return false;
            }

            if (fml == AddressFamily::IPv4) {
                IPAddress IPv4(IPAddress::parseIPv4(addr));
                if (IPv4 != IPAddress() || addr == "0.0.0.0")
                {
                    result.initIPv4(IPv4.addr());
                    return true;
                }
            } else if (fml == AddressFamily::IPv6) {
                IPAddress IPv6(IPAddress::parseIPv6(addr));
                if (IPv6 != IPAddress())
                {
                    result.initIPv6(IPv6.addr(), IPv6.scope());
                    return true;
                }
                return false;
            }

            return false;
        }

        bool
        IPAddress::tryDetermineFamily(const std::string& addr, AddressFamily& ret)
        {
            bool success = false;
            try {
                /* if (std::regex_match(addr, std::regex(IPV4_TEMPLATE))) { */
                /*     ret = AddressFamily::IPv4; */
                /*     success = true; */
                /* } else if (std::regex_match(addr, std::regex(IPV6_TEMPLATE))) { //TODO ! TEST */
                /*     ret = AddressFamily::IPv6; */
                /*     success = true; */
                /* } */
            } catch (std::regex_error& e) { //TODO!!!!!!!!!!!!!!
                assert(strcmp(e.what(), "Unexpected end of regex when escaping.") == 0);
                assert(e.code() == std::regex_constants::error_escape);
            }

            return success;
        }

        IPAddress& IPAddress::operator = (const IPAddress& other)
        {
            if (this == &other) {
                return *this;
            }

            if (_isIPv6) {
                std::memcpy(&_addrIPv6, &other._addrIPv6, sizeof(_addrIPv6));
                _scope = other._scope;
            } else {
                std::memcpy(&_addrIPv4, &other._addrIPv4, sizeof(_addrIPv4));
            }

            return *this;
        }

        /* cmp */
        bool IPAddress::operator ==(const IPAddress& other) const
        {
            if (family() == other.family()) {
                if (family() == AddressFamily::IPv4) {
                    return 0 == std::memcmp(&other._addrIPv4, &other._addrIPv4, sizeof(_addrIPv4));
                }
                else {
                    return _scope == other._scope && (0 == std::memcmp(&other._addrIPv6, &other._addrIPv6, sizeof(_addrIPv6)));
                }
            }
            return false;
        }

        bool IPAddress::operator !=(const IPAddress& other) const
        {
            return !(*this == other);
        }

        bool IPAddress::operator<(const IPAddress& a) const {
            sgx_socklen_t l1 = length();
            sgx_socklen_t l2 = a.length();
            if (l1 == l2) {
                if (scope() != a.scope()) return scope() < a.scope();
                return std::memcmp(addr(), a.addr(), l1) < 0;

            } else
                return l1 < l2;
        }

        bool IPAddress::operator<=(const IPAddress& a) const {
            return !(a < *this);
        }

        bool IPAddress::operator>(const IPAddress& a) const {
            return a < *this;
        }

        bool IPAddress::operator>=(const IPAddress& a) const {
            return !(*this < a);
        }

        /* IPAddress IPAddress::operator & (const IPAddress& addr) const */
        /* { */
        /*     if (family() == addr.family()) { */
        /*         if (family() == AddressFamily::IPv4) { */
        /*             IPAddress t(addr()); */
        /*             IPAddress o(addr.addr()); */
        /*             return IPAddress((t & o).addr(), sizeof(struct in_addr)); */
        /*         } else if (family() == AddressFamily::IPv6) { */
        /*             const IPAddress t(addr(), scope()); */
        /*             const IPAddress o(addr.addr(), addr.scope()); */
        /*             const IPAddress r = t & o; */
        /*             return IPAddress(r.addr(), sizeof(struct in6_addr), r.scope()); */
        /*         } */
        /*     } */
        /* } */

        /* IPAddress IPAddress::operator | (const IPAddress& addr) const */
        /* { */

        /* } */

        /* IPAddress IPAddress::operator ^ (const IPAddress& addr) const */
        /* { */

        /* } */

        /* IPAddress IPAddress::operator ~ () const */
        /* { */

        /* } */


        /* cmp address with Reserved special addresses */
        bool IPAddress::isWildcard() const
        {
            if (_isIPv6) {
                const uint16* words = reinterpret_cast<const uint16*>(&_addrIPv6);
                return words[0] == 0 && words[1] == 0 && words[2] == 0 && words[3] == 0 &&
                        words[4] == 0 && words[5] == 0 && words[6] == 0 && words[7] == 0;
            } else {
                return _addrIPv4.s_addr == INADDR_ANY;
            }
        }

        bool IPAddress::isBroadcast() const
        {
            return _isIPv6 ? false : _addrIPv4.s_addr == INADDR_NONE;
        }

        bool IPAddress::isLoopback() const
        {
            const uint16* words = reinterpret_cast<const uint16*>(&_addrIPv6);
            return _isIPv6 ?
                words[0] == 0 && words[1] == 0 && words[2] == 0 && words[3] == 0 &&
                words[4] == 0 && words[5] == 0 && words[6] == 0 && ByteOrder::toHost(words[7]) == 0x0001
                :
                (ntohl(_addrIPv4.s_addr) & 0xFF000000) == 0x7F000000;
        }

        bool IPAddress::isMulticast() const
        {
            const uint16* words = reinterpret_cast<const uint16*>(&_addrIPv6);
            return _isIPv6 ?
                (ByteOrder::toHost(words[0]) & 0xFFE0) == 0xFF00
                : (ntohl(_addrIPv4.s_addr) & 0xF0000000) == 0xE0000000;
        }

        /* bool IPAddress::isUnicast() const */
        /* { */
        /* } */

        bool IPAddress::isLinkLocal() const
        {
            const uint16* words = reinterpret_cast<const uint16*>(&_addrIPv6);
            return _isIPv6 ?
                    (ByteOrder::toHost(words[0]) & 0xFFE0) == 0xFE80
                :
                    (ntohl(_addrIPv4.s_addr) & 0xFFFF0000) == 0xA9FE0000;
        }

        bool IPAddress::isSiteLocal() const
        {
            const uint16* words = reinterpret_cast<const uint16*>(&_addrIPv6);
            uint32 addr = ntohl(_addrIPv4.s_addr);
            return _isIPv6 ?
                    ((ByteOrder::toHost(words[0]) & 0xFFE0) == 0xFEC0) || ((ByteOrder::toHost(words[0]) & 0xFF00) == 0xFC00)
                :
                    (addr & 0xFF000000) == 0x0A000000 ||        // 10.0.0.0/24
                    (addr & 0xFFFF0000) == 0xC0A80000 ||        // 192.68.0.0/16
                    (addr >= 0xAC100000 && addr <= 0xAC1FFFFF); // 172.16.0.0 to 172.31.255.255
        }

        bool IPAddress::isIPv4Compatible() const
        {
            const uint16* words = reinterpret_cast<const uint16*>(&_addrIPv6);
            return _isIPv6 ?
                words[0] == 0 && words[1] == 0 && words[2] == 0 && words[3] == 0 && words[4] == 0 && words[5] == 0
                : true;
        }

        bool IPAddress::isIPv4Mapped() const
        {
            const uint16* words = reinterpret_cast<const uint16*>(&_addrIPv6);
            return _isIPv6 ?
                words[0] == 0 && words[1] == 0 && words[2] == 0 && words[3] == 0 && words[4] == 0 && ByteOrder::toHost(words[5]) == 0xFFFF
                : true;
        }

        bool IPAddress::isWellKnownMC() const
        {
            const uint16* words = reinterpret_cast<const uint16*>(&_addrIPv6);
            return _isIPv6 ?
                    (ByteOrder::toHost(words[0]) & 0xFFF0) == 0xFF00
                :
                    (ntohl(_addrIPv4.s_addr) & 0xFFFFFF00) == 0xE0000000;
        }

        bool IPAddress::isNodeLocalMC() const
        {
            const uint16* words = reinterpret_cast<const uint16*>(&_addrIPv6);
            return _isIPv6 ?
                (ByteOrder::toHost(words[0]) & 0xFFEF) == 0xFF01
                :
                false;
        }

        bool IPAddress::isLinkLocalMC() const
        {
            const uint16* words = reinterpret_cast<const uint16*>(&_addrIPv6);
            return _isIPv6 ?
                    (ByteOrder::toHost(words[0]) & 0xFFEF) == 0xFF02
                :
                    (ntohl(_addrIPv4.s_addr) & 0xFF000000) == 0xE0000000;
        }

        bool IPAddress::isSiteLocalMC() const
        {
            const uint16* words = reinterpret_cast<const uint16*>(&_addrIPv6);
            return _isIPv6 ?
                    (ByteOrder::toHost(words[0]) & 0xFFEF) == 0xFF05
                :
                    (ntohl(_addrIPv4.s_addr) & 0xFFFF0000) == 0xEFFF0000;
        }

        bool IPAddress::isOrgLocalMC() const
        {
            const uint16* words = reinterpret_cast<const uint16*>(&_addrIPv6);
            return _isIPv6 ?
                    (ByteOrder::toHost(words[0]) & 0xFFEF) == 0xFF08
                :
                    (ntohl(_addrIPv4.s_addr) & 0xFFFF0000) == 0xEFC00000;
        }

        bool IPAddress::isGlobalMC() const
        {
            const uint16* words = reinterpret_cast<const uint16*>(&_addrIPv6);
            uint32 addr = ntohl(_addrIPv4.s_addr);
            return _isIPv6 ?
                    (ByteOrder::toHost(words[0]) & 0xFFEF) == 0xFF0F
                :
                    addr >= 0xE0000100 && addr <= 0xEE000000;
        }

        /* Inits */
        void IPAddress::initIPv4()
        {
            _isIPv6 = false;
            _scope = 0;
            std::memset(&_addrIPv4, 0, sizeof(_addrIPv4));
        }

        void IPAddress::initIPv4(const void* hostAddr)
        {
            _isIPv6 = false;
            _scope = 0;
            std::memcpy(&_addrIPv4, hostAddr, sizeof(_addrIPv4));
        }

        void IPAddress::initIPv4(unsigned prefix)
        {
            _isIPv6 = false;
            _scope = 0;
            sgx::uint32 addr = (prefix == 32) ? 0xffffffff : ~(0xffffffff >> prefix);
            _addrIPv4.s_addr = ByteOrder::toNetwork(addr);
        }

        void IPAddress::initIPv6()
        {
            _isIPv6 = true;
            _scope = 0;
            std::memset(&_addrIPv6, 0, sizeof(_addrIPv6));
        }

        void IPAddress::initIPv6(const void* hostAddr)
        {
            _isIPv6 = true;
            _scope = 0;
            std::memcpy(&_addrIPv6, hostAddr, sizeof(_addrIPv6));
        }

        void IPAddress::initIPv6(const void* hostAddr, sgx::uint32 scope)
        {
            _isIPv6 = true;
            _scope = scope;
            std::memcpy(&_addrIPv6, hostAddr, sizeof(_addrIPv6));
        }

        void IPAddress::initIPv6(unsigned prefix)
        {
            _isIPv6 = true;
            _scope = 0;
            unsigned i = 0;
            #ifdef _WIN32_
                for (; prefix >= 16; ++i, prefix -= 16) 
                {
                    _addrIPv6.s6_addr16[i] = 0xffff;
                }
                if (prefix > 0)
                {
                    _addrIPv6.s6_addr16[i++] = ByteOrder::toNetwork(static_cast<Poco::UInt16>(~(0xffff >> prefix)));
                }
                while (i < 8)
                {
                    _addrIPv6.s6_addr16[i++] = 0;
                }
            #else
                for (; prefix >= 32; ++i, prefix -= 32) 
                {
                    _addrIPv6.s6_addr32[i] = 0xffffffff;
                }
                if (prefix > 0)
                {
                    _addrIPv6.s6_addr32[i++] = ByteOrder::toNetwork(~(0xffffffffU >> prefix));
                }
                while (i < 4)
                {
                    _addrIPv6.s6_addr32[i++] = 0;
                }
            #endif
        }
    } /* Net */ 
} /* sgx */ 

