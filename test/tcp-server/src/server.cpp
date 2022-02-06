#include <server.hpp>

#include <string.h>

namespace TCP {
    std::map<std::string, in_addr_t>
    getLocalIP()
    {
        std::map<std::string, in_addr_t> addresses;
        char buff[64];
        hostent *sh;

        if (!gethostname(buff, sizeof(buff))) {
            if ( (sh = gethostbyname(buff)) ) {
                int interface_n = 0;
                while (sh->h_addr_list[interface_n]) {
                    struct sockaddr_in addr;
                    memcpy(&addr.sin_addr, sh->h_addr_list[interface_n], sh->h_length);
                    addresses[sh->h_aliases[interface_n]] = addr.sin_addr.s_addr;
                    interface_n++;
                }
            }
        }
        //cleanup TODO

        return addresses;
    }

    bool
    operator==(struct sockaddr_in a, struct sockaddr_in b)
    {
        if (a.sin_addr.s_addr == b.sin_addr.s_addr &&
                a.sin_family == b.sin_family &&
                a.sin_port == b.sin_port) {
            return true;
        }
        return false;
    }

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
        _use_ip6(useip6),
        _port(htons(port)),
        _socket(useip6 ? AF_INET6 : AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP),
        _data_hndl_fn(d_hndl_fn),
        _conn_hndl_fn(conn_hndl_fn),
        _disconn_hndl_fn(disconn_hdnl_fn),
        _thread_pool(threads)
    {
        //add ip6
        struct in_addr addr;
        inet_pton(AF_INET, ip.c_str(), &addr);
        _ip = addr.s_addr;
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

        memset(&_server_address.v4, 0, sizeof(_server_address.v4));
        _server_address.v4.sin_addr.s_addr = htonl(_ip);
        _server_address.v4.sin_port        = htons(_port);
        _server_address.v4.sin_family      = AF_INET;

        _socket.SetOpts(SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));
        _socket.Bind((struct sockaddr*)&(_server_address.v4.sin_addr), sizeof(_server_address.v4));

        _socket.Listen(1000); //TODO add var

        std::function<void()> hal = [this]() { handleAcceptLoop(); };
        std::function<void()> wdl = [this]() { waitDataLoop(); };
        _thread_pool.addJob(hal);
        _thread_pool.addJob(wdl);

        /* _use_ip6 = true; */
        /* struct in6_addr addr6; */
        /* inet_pton(AF_INET6, ip.c_str(), &addr6); */

        /* _server_address.v6.sin6_addr = addr6; */
        /* _server_address.v6.sin6_port = htons(port); */

        /* _server_address.sin_addr.s_addr = */ 

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
    Server::connectTo(const struct sockaddr_in host, connection_hndl_fn_t connect_hndl) {
        Socket client_socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (!client_socket.IsOK()) return false;

        if (_socket.Connect((sockaddr *)&host, sizeof(host)) != 0) {
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
    Server::disconnectHost(const struct sockaddr_in addr) {
        bool disconn = false;
        for (std::unique_ptr<Client>& client : _clients)
            if (client->getAddr().sin_addr.s_addr == addr.sin_addr.s_addr &&
                client->getAddr().sin_port == addr.sin_port) {
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
    Server::sendTo(const struct sockaddr_in addr, const IOBuff& data) {
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
    Server::enableKeepAlive(Socket socket) {
        int flag = 1;
        if (!socket.SetOpts(SOL_SOCKET, SO_KEEPALIVE, &flag, sizeof(flag))) return false;
        if (!socket.SetOpts(IPPROTO_TCP, TCP_KEEPIDLE, &_ka_conf.ka_idle, sizeof(_ka_conf.ka_idle))) return false;
        if (!socket.SetOpts(IPPROTO_TCP, TCP_KEEPINTVL, &_ka_conf.ka_intvl, sizeof(_ka_conf.ka_intvl))) return false;
        if (!socket.SetOpts(IPPROTO_TCP, TCP_KEEPCNT, &_ka_conf.ka_cnt, sizeof(_ka_conf.ka_cnt))) return false;
        return true;
    }

    void
    Server::handleAcceptLoop() {
        socklen_t addrlen = sizeof(sockaddr_in);
        struct sockaddr_in client_addr;
        Socket client_socket(
                _socket.Accept4((struct sockaddr*)&client_addr, &addrlen, SOCK_NONBLOCK)
            );

        if (client_socket.IsOK() && _status == ServerStatus::running) {
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
