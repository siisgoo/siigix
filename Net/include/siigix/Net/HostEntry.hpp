#ifndef HOSTENTRY_HPP_BK0VXEFJ
#define HOSTENTRY_HPP_BK0VXEFJ

#include "INetDefs.hpp"
#include "IPAddress.hpp"

#include <algorithm>
#include <string>
#include <vector>

namespace sgx {
    namespace Net {
        class HostEntry {
            public:
                using AliasList = std::vector<std::string>;
                using AddressList = std::vector<IPAddress>;

                HostEntry();
                HostEntry(const std::string& name, const IPAddress& addr);
                HostEntry(struct hostent*);
                HostEntry(struct addrinfo*);
                HostEntry(const HostEntry& other);
                HostEntry& operator = (const HostEntry& other);

                void swap(HostEntry& a);

                const std::string& name() const;
                const AliasList& aliases() const;
                const AddressList& addresses() const;

                virtual ~HostEntry();
            private:
                template <typename C>
                void removeDuplicates(C& list)
                {
                    std::sort(list.begin(), list.end());
                    auto last = std::unique(list.begin(), list.end());
                    list.erase(last, list.end());
                }

                std::string _name;
                AliasList   _aliases;
                AddressList _addresses;
        };

        inline const std::string&            HostEntry::name()      const { return _name;      }
        inline const HostEntry::AliasList&   HostEntry::aliases()   const { return _aliases;   }
        inline const HostEntry::AddressList& HostEntry::addresses() const { return _addresses; }

        inline void swap(HostEntry& a, HostEntry& b) { a.swap(b); }

    } /* Net */ 
} /* sgx  */ 

#endif /* end of include guard: HOSTENTRY_HPP_BK0VXEFJ */
