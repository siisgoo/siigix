#include <netdb.h>
#include <stdexcept>
#include <string.h>

template<class ProtocolC>
Server<ProtocolC>::Server(std::string host, port_t port, bool useip6, bool useudp, int max_conn, int threads) :
    Server(default_data_hndl_fn, default_conn_hndl_fn, default_conn_hndl_fn,
            host, port, useip6, useudp,
            max_conn,
            threads)
{ }

template<class ProtocolC>
Server<ProtocolC>::Server(data_hndl_fn_t d_hndl_fn,
        connection_hndl_fn_t conn_hndl_fn,
        connection_hndl_fn_t disconn_hdnl_fn,
        std::string host, port_t port, bool useip6, bool useudp,
        int max_conn, int threads) :
    _socket(host, port, { .ai_family = (useip6) ? AF_INET6 : AF_INET, .ai_socktype = (useudp) ? SOCK_DGRAM : SOCK_STREAM, .ai_protocol = 0 },
            { sock_opt(SOL_SOCKET, SO_REUSEADDR, USE_OPT_FLAG, sizeof(USE_OPT_FLAG)) }, max_conn),
    _data_hndl_fn(d_hndl_fn),
    _conn_hndl_fn(conn_hndl_fn),
    _disconn_hndl_fn(disconn_hdnl_fn),
    _thread_pool(threads)
{ }

template<class ProtocolC>
Server<ProtocolC>::~Server()
{
    if (_status == ServerStatus::running) {
        stop();
    }
}

template<class ProtocolC>
int
Server<ProtocolC>::start()
{
    if (_status == ServerStatus::running) {
        stop();
    }

    _status = ServerStatus::running;

    if (!_socket.isValid()) {
        _status = ServerStatus::shutdowned;
        throw std::domain_error(buildErrorMessage("Server::", __func__, ": _socket is invalid"));
    }

    _thread_pool.addJob([this]() { handleAcceptLoop(); });
    _thread_pool.addJob([this]() { waitDataLoop(); });

    return _status;
}

template<class ProtocolC>
void
Server<ProtocolC>::stop()
{
    _thread_pool.dropJobs();
    _status = ServerStatus::shutdowned;
    _socket.Close();
    _clients.clear();
}

template<class ProtocolC>
void Server<ProtocolC>::joinLoop() {_thread_pool.join();}

template<class ProtocolC>
bool
Server<ProtocolC>::connectClient(std::string ip, const port_t port, connection_hndl_fn_t connect_hndl) {
    std::unique_ptr<Client<ProtocolC>> client(new Client<ProtocolC>(ConnectSocket(ip, port)) );
    connect_hndl(*client);
    _client_mutex.lock();
    _clients.emplace_back(std::move(client));
    _client_mutex.unlock();

    return true;
}

template<class ProtocolC>
bool
Server<ProtocolC>::disconnectClient(std::string ip, port_t port) {
    bool disconn = false;
    for (std::unique_ptr<Client<ProtocolC>>& client : _clients)
        if (client->getIP() == ip && client->getPort() == port) {
            client->disconnect();
            disconn = true;
        }
    return disconn;
}

template<class ProtocolC>
void
Server<ProtocolC>::disconnectAll() {
    for(std::unique_ptr<Client<ProtocolC>>& client : _clients) {
        client->disconnect();
    }
}

template<class ProtocolC>
bool
Server<ProtocolC>::sendToClient(std::string ip, port_t port, const std::string& data) {
    bool data_is_sended = false;
    for (std::unique_ptr<Client<ProtocolC>>& client : _clients)
        if (client->getIP() == ip && client->getPort() == port) {
            client->sendMessage(data);
            data_is_sended = true;
    }
    return data_is_sended;
}

template<class ProtocolC>
void
Server<ProtocolC>::sendToAll(const std::string& data) {
    for(std::unique_ptr<Client<ProtocolC>>& client : _clients)
        client->sendMessage(data);
}

template<class ProtocolC>
void
Server<ProtocolC>::handleAcceptLoop() {
    if (_status == ServerStatus::running) {
        /* TODO AHTUNG!!! not clear memeory on error?!!!!!!!!!!!!!! */
        std::unique_ptr<Client<ProtocolC>> client( new Client<ProtocolC>(_socket.Accept(SOCK_NONBLOCK)) );
        _conn_hndl_fn(*client);
        _client_mutex.lock();
        _clients.emplace_back(std::move(client));
        _client_mutex.unlock();
    }

    //loop
    if (_status == ServerStatus::running) {
        _thread_pool.addJob([this](){handleAcceptLoop();});
    }
}

template<class ProtocolC>
void
Server<ProtocolC>::waitDataLoop()
{
    {
        std::lock_guard lock(_client_mutex);

        /* for all clients */
        for (auto it = _clients.begin(), end = _clients.end(); it != end; ++it)
        {
            auto& cur_client = *it;

            if (cur_client && cur_client->_status == ClientStatus::connected) /* client reciving data */
            {
                std::string data;
                cur_client->recvMessage(data);
                if (data.length() > 0)
                {
                    _thread_pool.addJob(
                        [this, _data = std::move(data), &cur_client]()
                        {
                            cur_client->_access_mutex.lock();
                            _data_hndl_fn(std::move(_data), *cur_client);
                            cur_client->_access_mutex.unlock();
                        }
                    );
                }
            }
            else if (cur_client->_status == ClientStatus::disconnected) /* on client disconnected */
            {
                _thread_pool.addJob(
                    [this, &cur_client, it] ()
                    {
                        cur_client->_access_mutex.lock();
                        Client<ProtocolC>* cli_p = cur_client.release();
                        cur_client = nullptr;
                        cli_p->_access_mutex.unlock();
                        _disconn_hndl_fn(*cli_p);
                        _clients.erase(it);
                        delete cli_p;
                   }
               );
            }
        }
    }

    //loop
    if (_status == ServerStatus::running) {
        _thread_pool.addJob([this](){waitDataLoop();});
    }
}
