#ifndef DNS_HPP_JFU0PBTS
#define DNS_HPP_JFU0PBTS

#include "INetDefs.hpp"
#include "HostEntry.hpp"
#include "IPAddress.hpp"

#include <string>

namespace sgx {
    namespace Net {

        class DNS {
            public:
                enum HintFlag
                {
                    DNS_HINT_NONE              = 0,
                    DNS_HINT_AI_PASSIVE        = AI_PASSIVE,  // Socket address will be used in bind() call
                    DNS_HINT_AI_CANONNAME   = AI_CANONNAME,   // Return canonical name in first ai_canonname
                    DNS_HINT_AI_NUMERICHOST = AI_NUMERICHOST, // Nodename must be a numeric address string
                    DNS_HINT_AI_NUMERICSERV = AI_NUMERICSERV, // Servicename must be a numeric port number
                    DNS_HINT_AI_ALL         = AI_ALL,         // Query both IP6 and IP4 with AI_V4MAPPED
                    DNS_HINT_AI_ADDRCONFIG  = AI_ADDRCONFIG,  // Resolution only if global address configured
                    DNS_HINT_AI_V4MAPPED    = AI_V4MAPPED     // On v6 failure, query v4 and convert to V4MAPPED format
                };

                static HostEntry hostByName(const std::string& hostname, unsigned hintFlags = DNS_HINT_AI_CANONNAME | DNS_HINT_AI_ADDRCONFIG);
                static HostEntry hostByAddress(const IPAddress& address, unsigned hintFlags = DNS_HINT_AI_CANONNAME | DNS_HINT_AI_ADDRCONFIG);
                static HostEntry resolve(const std::string& hostname);
                static IPAddress resolveOne(const std::string& hostname);
                static std::string hostName();
                static HostEntry thisHost();

                /* static std::string encodeIDN(const std::string& idn); */
                /* static std::string decodeIDN(const std::string& encodedIDN); */

                static bool isIDN(const std::string& hn);

                virtual ~DNS() {  }

            protected:
                /* static std::string encodeIDNLabel(const std::string& idn); */
                /* static std::string decodeIDNLabel(const std::string& encodedIDN); */

            private:
                DNS() {  }
        };
    } /* Net */ 
} /* sgx  */

#endif /* end of include guard: DNS_HPP_JFU0PBTS */
