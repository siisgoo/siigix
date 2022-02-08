#include <Server.hpp>

namespace siigix {
namespace TCP {
    Server::Client::Client(DataSocket&& sock) :
        _socket(std::move(sock)),
        _protocol(_socket)
    {
        /* _socket.EnableKeepAlive(); */
    }

    Server::Client::~Client()
    {
        this->disconnect();
    }

    int
    Server::Client::disconnect()
    {
        _status = ClientStatus::disconnected;
        if (!_socket.isValid()) {
            return _status;
        }
        _socket.Close();
        return _status;
    }

    bool
    Server::Client::recvMessage(std::string& data)
    {
        if(_status != ClientStatus::connected) {
            return false;
        }

        _protocol.recvMessage(data);

        if (data.length() <= 0) { //error
            disconnect();
            return false;
        }

        return true;
    }

    bool
    Server::Client::sendMessage(const std::string& data)
    {
        if (_status != ClientStatus::connected) {
            return false;
        }

        _protocol.sendMessage(_ip, data);

        return true;
    }

} /* TCP */ 
} /* siigix */ 
