#include <Server.hpp>

namespace siigix {
    namespace TCP {
        Server::Client::Client(TransSocket&& sock) :
            _socket(std::move(sock)),
                _proto(_socket)
        {
            _socket.EnableKeepAlive();
        }

        Server::Client::~Client()
        {
            this->disconnect();
        }

        void
        Server::Client::sendMessage(const std::string& data)
        {
            _proto.sendMessage(data);
        }

        void
        Server::Client::recvMessage(std::string& data)
        {
            _proto.recvMessage(data);

            if (data.size() <= 0) {
                disconnect();
            }
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

    } /* TCP */ 
} /* siigix */ 
