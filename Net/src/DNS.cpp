#include "siigix/Net/DNS.hpp"
#include "siigix/Net/HostEntry.hpp"
#include "siigix/Net/INetDefs.hpp"
#include "siigix/Net/SocketAddress.hpp"

#include <list>
#include <stdexcept>
#include <string>
#include <errno.h>

namespace sgx {
    namespace  Net {

        HostEntry
        DNS::hostByName(const std::string& hostname, unsigned hintFlags)
        {
            struct addrinfo* pAI;
            struct addrinfo hints;
            std::memset(&hints, 0, sizeof(hints));
            hints.ai_flags = hintFlags;
            int rc = getaddrinfo(hostname.c_str(), NULL, &hints, &pAI);
            if (rc == 0) {
                HostEntry result(pAI);
                freeaddrinfo(pAI);
                return result;
            }
            else {
                throw std::runtime_error(eprintf("DNS::", __func__, " geraddrinfo: ", strerror(errno)));
            }
            throw std::runtime_error(eprintf("DNS::", __func__, " cannot resolve host by name"));
        }

        HostEntry
        DNS::hostByAddress(const IPAddress& address, unsigned hintFlags)
        {
            SocketAddress sa(address, 0);
            char fqname[1024];
            int rc = getnameinfo(sa.addr(), sa.length(), fqname, sizeof(fqname), NULL, 0, NI_NAMEREQD);
            if (rc == 0) {
                struct addrinfo* pAI;
                struct addrinfo hints;
                std::memset(&hints, 0, sizeof(hints));
                hints.ai_flags = hintFlags;
                rc = getaddrinfo(fqname, NULL, &hints, &pAI);
                if (rc == 0) {
                    HostEntry result(pAI);
                    freeaddrinfo(pAI);
                    return result;
                }
                else {
                    throw std::runtime_error(eprintf("HostEntry::", __func__, " getaddrinfo: ", strerror(errno)));
                }
            }
            else {
                throw std::runtime_error(eprintf("HostEntry::", __func__, " getaddrinfo: ", strerror(errno)));
            }
        }

        HostEntry
        DNS::resolve(const std::string& address)
        {
            IPAddress ip;
            if (IPAddress::tryParse(address, ip)) {
                return hostByAddress(ip);
            } else if (isIDN(address)) {
                /* std::string encoded = encodeIDN(address); */
                /* return hostByName(encoded); */
                throw std::runtime_error(eprintf("DNS::", __func__, " Cant handle IDN DNS names!"));
            } else {
                return hostByName(address);
            }
        }

        IPAddress
        DNS::resolveOne(const std::string& address)
        {
            const HostEntry& entry = resolve(address);
            if (!entry.addresses().empty())
                return entry.addresses()[0];
            else
                throw std::runtime_error(eprintf("DNS::", __func__, " Cant resolve address: ", address.c_str()));
        }


        std::string
        DNS::hostName()
        {
            char buffer[256];
            int rc = gethostname(buffer, sizeof(buffer));
            if (rc == 0)
                return std::string(buffer);
            else
                throw std::runtime_error(eprintf("DNS::", __func__, " gethostname: ", strerror(errno)));
        }

        HostEntry
        DNS::thisHost()
        {
            return hostByName(hostName());
        }


        bool DNS::isIDN(const std::string& hostname)
        {
            for (auto ch: hostname)
            {
                if (static_cast<unsigned char>(ch) >= 0x80) return true;
            }
            return false;
        }

        /* std::string DNS::encodeIDN(const std::string& idn) */
        /* { */
        /*     std::string encoded; */
        /*     std::string::const_iterator it = idn.begin(); */
        /*     std::string::const_iterator end = idn.end(); */
        /*     while (it != end) { */
        /*         std::string label; */
        /*         bool mustEncode = false; */
        /*         while (it != end && *it != '.') { */
        /*             if (static_cast<unsigned char>(*it) >= 0x80) mustEncode = true; */
        /*             label += *it++; */
        /*         } */

        /*         if (mustEncode) */
        /*             encoded += encodeIDNLabel(label); */
        /*         else */
        /*             encoded += label; */
        /*         if (it != end) encoded += *it++; */
        /*     } */
        /*     return encoded; */
        /* } */

        /* std::string DNS::decodeIDN(const std::string& encodedIDN) */
        /* { */
        /*     std::string decoded; */
        /*     std::string::const_iterator it = encodedIDN.begin(); */
        /*     std::string::const_iterator end = encodedIDN.end(); */
        /*     while (it != end) */
        /*     { */
        /*         std::string label; */
        /*         while (it != end && *it != '.') */
        /*         { */
        /*         label += *it++; */
        /*         } */
        /*         decoded += decodeIDNLabel(label); */
        /*         if (it != end) decoded += *it++; */
        /*     } */
        /*     return decoded; */
        /* } */


        /* std::string */
        /* DNS::encodeIDNLabel(const std::string& idn) */
        /* { */
        /*     std::string encoded = "xn--"; */
        /*     std::vector<sgx::uint32> uniLabel; */
        /*     sgx::UTF8Encoding utf8; */
        /*     sgx::TextIterator it(label, utf8); */
        /*     sgx::TextIterator end(label); */
        /*     while (it != end) */
        /*     { */
        /*         int ch = *it; */
        /*         if (ch < 0) throw std::runtime_error(eprintf("DNS::", __func__, " Invalid UTF-8 character in IDN label: ", label)); */
        /*         if (sgx::Unicode::isUpper(ch)) */
        /*         { */
        /*             ch = sgx::Unicode::toLower(ch); */
        /*         } */
        /*         uniLabel.push_back(static_cast<sgx::uint32>(ch)); */
        /*         ++it; */
        /*     } */
        /*     char buffer[64]; */
        /*     std::size_t size = 64; */
        /*     int rc = punycode_encode(uniLabel.size(), &uniLabel[0], &size, buffer); */
        /*     if (rc == punycode_success) { */
        /*         encoded.append(buffer, size); */
        /*     } */
        /*     else { */
        /*         throw std::runtime_error(eprintf("DNS::", __func__, " Failed to encode IDN label: ", label)); */
        /*     } */

        /*     return encoded; */
        /* } */

        /* std::string */
        /* DNS::decodeIDNLabel(const std::string& encodedIDN) */
        /* { */
        /*     std::string decoded; */
        /*     if (encodedIDN.compare(0, 4, "xn--") == 0) */
        /*     { */
        /*         std::size_t size = 64; */
        /*         punycode_uint buffer[64]; */
        /*         int rc = punycode_decode(encodedIDN.size() - 4, encodedIDN.data() + 4, &size, buffer); */
        /*         if (rc == punycode_success) */
        /*         { */
        /*             sgx::UTF32Encoding utf32; */
        /*             sgx::UTF8Encoding utf8; */
        /*             sgx::TextConverter converter(utf32, utf8); */
        /*             converter.convert(buffer, static_cast<int>(size*sizeof(punycode_uint)), decoded); */
        /*         } */
        /*         else throw std::runtime_error(eprintf("DNS::", __func__, " Failed to decode IDN label: ", encodedIDN)); */
        /*     } */
        /*     else */
        /*     { */
        /*         decoded = encodedIDN; */
        /*     } */
        /*     return decoded; */
        /* } */

    } /*  Net */ 
} /* sgx */ 
