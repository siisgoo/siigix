#ifndef PROTOCOL_HPP_NWSF605U
#define PROTOCOL_HPP_NWSF605U

#include "Socket.hpp"

#include <string>

namespace siigix {

    class IProtocol {
        public:
            IProtocol(DataSocket& ds);
            virtual ~IProtocol();

            virtual void sendMessage(std::string url, const IOBuff& data) = 0;
            virtual void recvMessage(IOBuff& data) = 0;
        protected:
            DataSocket& _socket;
    };

    namespace TCP {
        class Protocol : public IProtocol {
            public:
                using IProtocol::IProtocol;
                virtual void sendMessage(std::string url, const IOBuff& data) override;
                virtual void recvMessage(IOBuff& data) override;
        };
    }; /* TCP */

    /* namespace HTTP { */
    /*     class Protocol: public IProtocol { */
    /*         public: */
    /*             virtual void sendMessage(std::string url, const IOBuff& data) override; */
    /*             virtual void recvMessage(IOBuff& data) override; */
    /*     }; */
    /* }; /1* HTTP *1/ */

} /* siigix */ 

#endif /* end of include guard: PROTOCOL_HPP_NWSF605U */
