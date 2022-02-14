#include <HostEntry.hpp>
#include <assert.h>

namespace sgx {
    namespace Net {

        HostEntry::~HostEntry()
        {
        }

        HostEntry::HostEntry()
        {
        }

        HostEntry::HostEntry(struct hostent* entry)
        {
            /* assert(entry == nullptr); */

            _name = entry->h_name;
            char** alias = entry->h_aliases;
            if (alias)
            {
                while (*alias)
                {
                    _aliases.push_back(std::string(*alias));
                    ++alias;
                }
            }
            removeDuplicates(_aliases);

            char** address = entry->h_addr_list;
            if (address)
            {
                while (*address)
                {
                    _addresses.push_back(IPAddress(*address, entry->h_length));
                    ++address;
                }
            }
            removeDuplicates(_addresses);
        }

        HostEntry::HostEntry(struct addrinfo *ainfo)
        {
            for (struct addrinfo* ai = ainfo; ai; ai = ai->ai_next)
            {
                if (ai->ai_canonname)
                {
                    _name.assign(ai->ai_canonname);
                }
                if (ai->ai_addrlen && ai->ai_addr)
                {
                    switch (ai->ai_addr->sa_family)
                    {
                        case AF_INET:
                            _addresses.push_back(IPAddress(&reinterpret_cast<struct sockaddr_in*>(ai->ai_addr)->sin_addr, sizeof(in_addr)));
                            break;
                        case AF_INET6:
                            _addresses.push_back(IPAddress(&reinterpret_cast<struct sockaddr_in6*>(ai->ai_addr)->sin6_addr, sizeof(in6_addr), reinterpret_cast<struct sockaddr_in6*>(ai->ai_addr)->sin6_scope_id));
                            break;
                    }
                }
            }
            removeDuplicates(_addresses);
        }

        HostEntry::HostEntry(const std::string& name, const IPAddress& addr)
            : _name(name)
        {
            _addresses.push_back(addr);
        }

        HostEntry::HostEntry(const HostEntry& other) :
            _name(other._name),
            _aliases(other._aliases),
            _addresses(other._addresses)
        { }

        HostEntry& HostEntry::operator = (const HostEntry& other)
        {
            if (&other != this)
            {
                _name      = other._name;
                _aliases   = other._aliases;
                _addresses = other._addresses;
            }
            return *this;
        }

        void HostEntry::swap(HostEntry& hostEntry)
        {
            std::swap(_name, hostEntry._name);
            std::swap(_aliases, hostEntry._aliases);
            std::swap(_addresses, hostEntry._addresses);
        }

    } /* Net */
} /* sgx  */ 
