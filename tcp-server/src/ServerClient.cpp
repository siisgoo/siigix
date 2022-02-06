#include <Server.hpp>

namespace siigix {
namespace TCP {
    Server::Client::Client(INet::Socket socket, INet::sockaddr_in_any addr) :
        _cli_socket(socket),
        _address(addr)
    { }

    Server::Client::~Client()
    {
        this->disconnect();
    }

    sockaddr_in_any
    Server::Client::getAddr() const {
        return _address;
    }

    ClientStatus
    Server::Client::disconnect()
    {
        _status = ClientStatus::disconnected;
        if (!_cli_socket.IsUP()) return _status;
        /* _cli_socket.Shutdown(SD_BOTH); */
        _cli_socket.Shutdown(SHUT_RDWR);
        _cli_socket.Close();
        return _status;
    }

    IOBuff
    Server::Client::recive() {
        if(_status != ClientStatus::connected) return IOBuff();
        using namespace std::chrono_literals;
        IOBuff data;
        size_t recv_len = 1024;
        uint32_t size;
        int err;

        data.resize(recv_len);
        int answ = _cli_socket.Recv(data, recv_len, MSG_DONTWAIT);

        // Disconnect
        if (!answ) {
            disconnect();
            return IOBuff();
        } else if(answ == -1) {
            socklen_t len = sizeof (err);
            _cli_socket.GetOpts(SOL_SOCKET, SO_ERROR, &err, &len);
            if(!err) err = errno;
            switch (err) {
                case 0: break;
                // Keep alive timeout
                case ETIMEDOUT:
                case ECONNRESET:
                case EPIPE:
                    disconnect();
                    [[fallthrough]];
                case EAGAIN: return IOBuff();
                default:
                    disconnect();
                    //log
                    /* std::cerr << "Unhandled error!\n" */
                    /*  << "Code: " << err << " Err: " << std::strerror(err) << '\n'; */
                return IOBuff();
            }
        }

        return data;
    }

    bool
    Server::Client::send(const IOBuff& data) const {
        if (_status != ClientStatus::connected) return false;

        if (!_cli_socket.Send(data, 0)) return false;

        return true;
    }

} /* TCP */ 
} /* siigix */ 
