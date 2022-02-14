#include <Server.hpp>
#include <Protocol.hpp>

namespace sgx {
namespace Net {

    ServerTCP::Client::Client(TransSocket&& sock, ProtocolFactory* factory) :
        _socket(std::move(sock)),
        _protocol(factory),
        _status(ClientStatus::connected)
    {
        _socket.EnableKeepAlive();
    }

    ServerTCP::Client::~Client()
    {
        this->disconnect();
    }

    void
    ServerTCP::Client::sendMessage(const std::string& data)
    {
        _protocol.sendMessage(_socket, data);
    }

    void
    ServerTCP::Client::reciveMessage(std::string& data)
    {
        _protocol.reciveMessage(_socket, data);

        if (data.size() <= 0) {
            disconnect();
        }
    }

    int
    ServerTCP::Client::disconnect()
    {
        _status = ClientStatus::disconnected;
        if (!_socket.isValid()) {
            return _status;
        }
        _socket.Close();
        return _status;
    }

} /* Net */ 
} /* sgx  */ 
