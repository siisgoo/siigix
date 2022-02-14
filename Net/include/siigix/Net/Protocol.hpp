#ifndef PROTOCOL_HPP_NWSF605U
#define PROTOCOL_HPP_NWSF605U

#include "Socket.hpp"

#include <memory>
#include <string>

namespace sgx {
    namespace Net {

        /* Implementation */
        class IProtocol {
            public:
                IProtocol();

                virtual void sendMessage(TransSocket& sock, const std::string& data) = 0;
                virtual void reciveMessage(TransSocket& sock, std::string& data) = 0;

                virtual ~IProtocol();
        };

        /* simple protocol */
        class ProtocolSimple : public IProtocol {
            public:
                using IProtocol::IProtocol;

                virtual void sendMessage(TransSocket& sock, const std::string& data) override;
                virtual void reciveMessage(TransSocket& sock, std::string& data) override;
        };

        /* http protocol */
        class ProtocolHTTP : public IProtocol {
            public:
                using IProtocol::IProtocol;

                virtual void sendMessage(TransSocket& sock, const std::string& data) override;
                virtual void reciveMessage(TransSocket& sock, std::string& data) override;
        };

        /* Factories */
        class ProtocolFactory {
            public:
                virtual ~ProtocolFactory() {  }
                virtual IProtocol * buildProtocol() const = 0;
        };

        class ProtocolSimpleFactory : public ProtocolFactory {
            public:
                virtual ~ProtocolSimpleFactory() {  }
                virtual IProtocol * buildProtocol() const override;
        };

        class ProtocolHTTPFactory : public ProtocolFactory {
            public:
                virtual ~ProtocolHTTPFactory() {  }
                virtual IProtocol * buildProtocol() const override;
        };

        /* Handle all protocols implementations (container) */
        class Protocol : public IProtocol {
            public:
                Protocol(ProtocolFactory*);

                void sendMessage(TransSocket& sock, const std::string& data);
                void reciveMessage(TransSocket& sock, std::string& data);

                virtual ~Protocol();
            protected:
                IProtocol *_impl;
        };

        /* Protocol Interface class */
        inline IProtocol::IProtocol() {  }
        inline IProtocol::~IProtocol() { }

        /* Factory ProtocolSimple */
        inline IProtocol * ProtocolSimpleFactory::buildProtocol() const {
            return new ProtocolSimple();
        }

        /* Factory ProtocolHTTP */
        inline IProtocol * ProtocolHTTPFactory::buildProtocol() const {
            return new ProtocolHTTP();
        }

        inline Protocol::Protocol(ProtocolFactory* spec) {
            _impl = spec->buildProtocol();
        }

        inline Protocol::~Protocol() { delete _impl;  }

        inline void Protocol::sendMessage(TransSocket& sock, const std::string& data) {
            _impl->sendMessage(sock, data);
        }

        inline void Protocol::reciveMessage(TransSocket& sock, std::string& data) {
            _impl->reciveMessage(sock, data);
        }

    } /* Net */ 

} /* sgx */ 

#endif /* end of include guard: PROTOCOL_HPP_NWSF605U */
