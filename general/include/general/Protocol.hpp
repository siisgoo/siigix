#ifndef PROTOCOL_HPP_NWSF605U
#define PROTOCOL_HPP_NWSF605U

#include "Socket.hpp"

#include <string>

namespace siigix {

    class IProtocol {
        public:
            IProtocol(TransSocket& ds);
            virtual ~IProtocol();

            virtual void sendMessage(const std::string& data) = 0;
            virtual void recvMessage(std::string& data) = 0;
        protected:
            TransSocket& _socket;
    };

    namespace TCP {
        class Protocol : public IProtocol {
            public:
                using IProtocol::IProtocol;
                virtual void sendMessage(const std::string& data) override;
                virtual void recvMessage(std::string& data) override;
        };

        /* namespace HTTP { */
        /*     class Protocol: public TCP::Protocol { */
        /*         public: */
        /*             struct header { */
        /*                 const char *name; */
        /*                 const char *value; */
        /*             }; */

        /*             /1* METHOD URI HTTP/VERSION *1/ */
        /*             struct request_line { */
        /*                 const char *method; */
        /*                 const char *uri; */
        /*                 const char *version; */
        /*             }; */

        /*             struct Request { */
        /*                 struct request_line; */
        /*                 struct header headers[100]; //TODO handle this moment */
        /*             }; */

        /*             struct Message { */
        /*                 struct Request; */
        /*             }; */

        /*         public: */
        /*             Protocol(TransSocket& socket); */
        /*             virtual ~Protocol(); */

        /*             void recvRequest(); */
        /*             void sendResponce(); */
        /*             /1* virtual void parceRequest(std::string& request); *1/ */
        /*             /1* virtual void createResponce(std::string& request); *1/ */
        /*         private: */
        /*             virtual void sendMessage(std::string url, const std::string& data) override; */
        /*             virtual void recvMessage(std::string& data) override; */

        /*             void parceRequest(); */
        /*             std::string createResponce(); */
        /*     }; */
        /* }; /1* HTTP *1/ */

    }; /* TCP */

} /* siigix */ 

#endif /* end of include guard: PROTOCOL_HPP_NWSF605U */
