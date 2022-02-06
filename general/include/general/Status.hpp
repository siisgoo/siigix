#ifndef STATUS_HPP_UO860IYO
#define STATUS_HPP_UO860IYO

#include "BitFlag.hpp"

namespace siigix {
    namespace TCP {
        enum ServerStatus {
            running,
            shutdowned,
            ERR_SERVER_SOCKET, //deligate diagnostic to Socket class
        };

        enum ClientStatus {
            connected,
            disconnected,
            ERR_CLI_SOCKET, //deligate diagnostic to Socket class
        };
    } /* TCP */ 
} /* siigix */ 

#endif /* end of include guard: STATUS_HPP_UO860IYO */
