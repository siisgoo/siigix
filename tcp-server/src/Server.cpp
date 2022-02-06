#include <Server.hpp>

#include <string.h>

namespace siigix
{
namespace TCP {
    Server::Server(data_hndl_fn_t data_hndl_fn,
            bool useip6, int port, std::string ip, int threads) :
        Server(data_hndl_fn, default_conn_hndl_fn, default_conn_hndl_fn,
                useip6, port, ip,
                threads)
    { }

    Server::Server(data_hndl_fn_t d_hndl_fn,
            connection_hndl_fn_t conn_hndl_fn,
            connection_hndl_fn_t disconn_hdnl_fn,
            bool useip6, int port, std::string ip,
            int threads) :
        _server_address(useip6),
        _socket(useip6 ? AF_INET6 : AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP),
        _data_hndl_fn(d_hndl_fn),
        _conn_hndl_fn(conn_hndl_fn),
        _disconn_hndl_fn(disconn_hdnl_fn),
        _thread_pool(threads)
    {
        if (useip6) { //todo
            /* memset(&_server_address.ip4, 0, sizeof(_server_address.ip4)); */
            /* struct in6_addr addr; */
            /* inet_pton(AF_INET6, ip.c_str(), &addr); */
            /* _server_address.ip6.sin6_port = htons(port); */
            /* _server_address.ip6.sin6_addr = addr; //strcpy? */
            /* _server_address.ip6.sin6_family = AF_INET6; */
        } else {
            struct in_addr addr;
            inet_pton(AF_INET6, ip.c_str(), &addr);
            memset(&_server_address.ip4(), 0, sizeof(_server_address.ip4()));
            _server_address.ip4().sin_port = htons(port);
            _server_address.ip4().sin_addr = addr;
            _server_address.ip4().sin_family = AF_INET;
        }
    }

    Server::~Server()
    {
        if (_status == ServerStatus::running) {
            stop();
        }
    }

    ServerStatus
    Server::start()
    {
        if (_status == ServerStatus::running) {
            stop();
        }

        /////////////////ADD CHECKS////////////////////////////

        int reuseaddr = 1;

        _socket.SetOpts(SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));
        _socket.Bind((struct sockaddr*)&(_server_address.ip4().sin_addr), sizeof(_server_address.ip4()));

        _socket.Listen(1000); //TODO add var

        std::function<void()> hal = [this]() { handleAcceptLoop(); };
        std::function<void()> wdl = [this]() { waitDataLoop(); };
        _thread_pool.addJob(hal);
        _thread_pool.addJob(wdl);

        return _status;
    }

    void
    Server::stop()
    {
        _thread_pool.dropJobs();
        _status = ServerStatus::shutdowned;
        _socket.Close();
        _clients.clear();
    }

    void Server::joinLoop() {_thread_pool.join();}

    bool
    Server::connectTo(const INet::sockaddr_in_any host, connection_hndl_fn_t connect_hndl) {
        INet::Socket client_socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (client_socket.IsUP() != INet::Socket::Status::up) return false;

        if (_socket.Connect((sockaddr *)&std::move(host.ip4()), sizeof(host)) != 0) { //add conversion of host.ip4()
            client_socket.Close();
            return false;
        }

        if(!enableKeepAlive(client_socket)) {
            client_socket.Shutdown(0);
            client_socket.Close();
        }

        std::unique_ptr<Client> client(new Client(client_socket, host));
        connect_hndl(*client);
        _client_mutex.lock();
        _clients.emplace_back(std::move(client));
        _client_mutex.unlock();
        return true;
    }

    bool
    Server::disconnectHost(const INet::sockaddr_in_any addr) {
        bool disconn = false;
        for (std::unique_ptr<Client>& client : _clients)
            if (client->getAddr() == addr) {
                client->disconnect();
                disconn = true;
            }
        return disconn;
    }

    void
    Server::disconnectAll() {
        for(std::unique_ptr<Client>& client : _clients) {
            client->disconnect();
        }
    }

    bool
    Server::sendTo(const INet::sockaddr_in_any addr, const IOBuff& data) {
        bool data_is_sended = false;
        for (std::unique_ptr<Client>& client : _clients)
            if (client->getAddr() == addr) {
                client->send(data);
                data_is_sended = true;
        }
        return data_is_sended;
    }

    void
    Server::send(const IOBuff& data) {
        for(std::unique_ptr<Client>& client : _clients)
            client->send(data);
    }

    bool
    Server::enableKeepAlive(INet::Socket socket) {
        int flag = 1;
        if (!socket.SetOpts(SOL_SOCKET, SO_KEEPALIVE, &flag, sizeof(flag))) return false;
        if (!socket.SetOpts(IPPROTO_TCP, TCP_KEEPIDLE, &_ka_conf.ka_idle, sizeof(_ka_conf.ka_idle))) return false;
        if (!socket.SetOpts(IPPROTO_TCP, TCP_KEEPINTVL, &_ka_conf.ka_intvl, sizeof(_ka_conf.ka_intvl))) return false;
        if (!socket.SetOpts(IPPROTO_TCP, TCP_KEEPCNT, &_ka_conf.ka_cnt, sizeof(_ka_conf.ka_cnt))) return false;
        return true;
    }

    void
    Server::handleAcceptLoop() {
        socklen_t addrlen = sizeof(struct sockaddr_in);
        INet::sockaddr_in_any client_addr;
        INet::Socket client_socket(
                _socket.Accept4((struct sockaddr*)&client_addr.ip4(), &addrlen, SOCK_NONBLOCK)
            );

        if (client_socket.IsUP() == INet::Socket::Status::up && _status == ServerStatus::running) {
            if (enableKeepAlive(client_socket)) {
                std::unique_ptr<Client> client(new Client(client_socket, client_addr));
                _conn_hndl_fn(*client);
                _client_mutex.lock();
                _clients.emplace_back(std::move(client));
                _client_mutex.unlock();
            } else {
                client_socket.Shutdown(0);
                client_socket.Close();
            }
        }

        //loop
        if (_status == ServerStatus::running) {
            _thread_pool.addJob([this](){handleAcceptLoop();});
        }
    }

    void
    Server::waitDataLoop()
    {
        std::lock_guard lock(_client_mutex);
        for (auto it = _clients.begin(), end = _clients.end(); it != end; ++it)
        {
            auto& client = *it;

            if (client)
            {
                if (IOBuff data = client->recive(); data.len() > 0)
                {
                    _thread_pool.addJob([this, _data = std::move(data), &client]() {
                        client->_access_mutex.lock();
                        _data_hndl_fn(std::move(_data), *client);
                        client->_access_mutex.unlock();
                    });
                }
            }
            else if(client->_status == ClientStatus::disconnected)
            {
                _thread_pool.addJob([this, &client, it] () {
                    client->_access_mutex.lock();
                    Client* pointer = client.release();
                    client = nullptr;
                    pointer->_access_mutex.unlock();
                    _disconn_hndl_fn(*pointer);
                    _clients.erase(it);
                    delete pointer;
               });
            }
        }

        //loop
        if (_status == ServerStatus::running) {
            _thread_pool.addJob([this](){waitDataLoop();});
        }
    }

} /* TCP */ 
} /* siigix */ 
