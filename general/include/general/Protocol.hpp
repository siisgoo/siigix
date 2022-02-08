#ifndef PROTOCOL_HPP_NWSF605U
#define PROTOCOL_HPP_NWSF605U

#include "Socket.hpp"

#include <string>

namespace siigix {

    class IProtocol {
        public:
            IProtocol(DataSocket& ds);
            virtual ~IProtocol();

            virtual void sendMessage(std::string url, const std::string& data) = 0;
            virtual void recvMessage(std::string& data) = 0;
        protected:
            DataSocket& _socket;
    };

    namespace TCP {
        class Protocol : public IProtocol {
            public:
                using IProtocol::IProtocol;
                virtual void sendMessage(std::string url, const std::string& data) override;
                virtual void recvMessage(std::string& data) override;
        };

        namespace HTTP {
            class Protocol: public TCP::Protocol {
                public:
                    struct header {
                        const char *name;
                        const char *value;
                    };

                    /* METHOD URI HTTP/VERSION */
                    struct request_line {
                        const char *method;
                        const char *uri;
                        const char *version;
                    };

                    struct Request {
                        struct request_line;
                        struct header headers[100]; //TODO handle this moment
                    };

                    struct Message {
                        struct Request;
                    };

                public:
                    void recvRequest();
                    void sendResponce();
                    /* virtual void parceRequest(std::string& request); */
                    /* virtual void createResponce(std::string& request); */
                private:
                    virtual void sendMessage(std::string url, const std::string& data) override;
                    virtual void recvMessage(std::string& data) override;

                    void parceRequest();
                    std::string createResponce();
            };
        }; /* HTTP */

    }; /* TCP */

} /* siigix */ 

#endif /* end of include guard: PROTOCOL_HPP_NWSF605U */
