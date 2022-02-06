#ifndef TYPES_HPP_PRGL2BYO
#define TYPES_HPP_PRGL2BYO

#include <sys/types.h>
#include <netinet/in.h>

namespace TCP {
    union sockaddr_any {
        struct sockaddr_in v4;
        struct sockaddr_in6 v6;
        sockaddr_any(struct sockaddr_in v) : v4(v) {}
        sockaddr_any(struct sockaddr_in6 v) : v6(v) {}
        sockaddr_any() {}
        ~sockaddr_any() {}
    };

    typedef __uint16_t inet_port_t;
}

#endif /* end of include guard: TYPES_HPP_PRGL2BYO */
