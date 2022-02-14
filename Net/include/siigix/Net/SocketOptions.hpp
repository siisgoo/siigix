#ifndef SOCKETOPTIONS_HPP_XSPLARQW
#define SOCKETOPTIONS_HPP_XSPLARQW

#include <siigix/General/eprintf.hpp>
#include <memory>
#include <iterator>
#include "INetDefs.hpp"

namespace sgx {
    namespace Net {

        #ifdef __linux__
            static int USE_OPT_FLAG = 1;
        #elif
            static char USE_OPT_FLAG = 1;
        #endif

        typedef struct SocketOption SocketOption;
        struct SocketOption {
            int level;
            int optValue;
            int optName;
            sgx_socklen_t optLen;

            SocketOption(int level, int opt, int value, int optlen) :
                level(level), optName(opt), optValue(value), optLen(optlen) {  }
            SocketOption() :
                SocketOption(0, 0, 0, 0) { }

            bool operator == (const SocketOption& other) const {
                return level==other.level &&
                        optValue==other.optValue &&
                        optName== other.optName &&
                        optLen==other.optLen;
            }
        };

        class SocketOptions {
            private:
                int _count;
                std::unique_ptr<SocketOption[]> _opts;

            public:

                class iterator : public std::iterator<std::input_iterator_tag,
                                                      SocketOption,
                                                      SocketOption,
                                                      const SocketOption*,
                                                      SocketOption>
                {
                        friend class SocketOptions;
                    private:
                        SocketOption *_option;
                    public:
                        explicit iterator(SocketOption* optp) : _option(optp) {  }
                        iterator& operator++() { _option++; return *this; }
                        bool operator == (const iterator& other) const { return _option == other._option; }
                        bool operator != (const iterator& other) const { return !(*this == other); }
                        reference operator*() const { return *_option; }
                };

                iterator begin() { return iterator(&(*this)[0]); };
                iterator end()   { return iterator(&(*this)[_count-1]); };

                const iterator begin() const { return iterator(&(*this)[0]); };
                const iterator end() const   { return iterator(&(*this)[_count-1]); };

                SocketOptions(int l_count)
                    : _count(l_count),
                    _opts(new SocketOption [l_count])
                { }

                SocketOptions(const std::initializer_list<SocketOption>& opts_list)
                    : SocketOptions(opts_list.size())
                {
                    int i = 0;
                    for (auto& opt: opts_list) {
                        memcpy(&_opts[i], &opt, sizeof(opt));
                        i++;
                    }
                }

                ~SocketOptions() { }

                int count() { return _count; };

                SocketOption& operator[](int i) const {
                    if (i >= 0 && i < _count) {
                        return _opts[i];
                    }
                    throw std::domain_error(eprintf("sock_opts::", __func__,
                                " Assign error. Assign to opts[] inside sock_opts not in range. passed value: ", i, ". Assignable range 0..", _count-1));
                }
        };

    } /* Net */ 
} /* sgx  */ 
#endif /* end of include guard: SOCKETOPTIONS_HPP_XSPLARQW */
