#include <Server.hpp>

#include <string.h>

namespace siigix
{
namespace TCP {
    Server::Server(port_t port, std::string ip, int threads) :
        Server(default_data_hndl_fn, default_conn_hndl_fn, default_conn_hndl_fn,
                port, ip,
                threads)
    { }

    Server::Server(data_hndl_fn_t d_hndl_fn,
            connection_hndl_fn_t conn_hndl_fn,
            connection_hndl_fn_t disconn_hdnl_fn,
            port_t port, std::string ip, int threads) :
        _socket(port, { sock_opt(SOL_SOCKET, SO_REUSEADDR, USE_OPT_FLAG, sizeof(USE_OPT_FLAG)) }),
        _data_hndl_fn(d_hndl_fn),
        _conn_hndl_fn(conn_hndl_fn),
        _disconn_hndl_fn(disconn_hdnl_fn),
        _thread_pool(threads)
    { }

    Server::~Server()
    {
        if (_status == ServerStatus::running) {
            stop();
        }
    }

    int
    Server::start()
    {
        if (_status == ServerStatus::running) {
            stop();
        }

        _status = ServerStatus::running;

        if (!_socket.isValid()) {
            _status = ServerStatus::shutdowned;
            throw "SOCKET ERRRRRRRRRRRRRRROR!";
        }

        _thread_pool.addJob([this]() { handleAcceptLoop(); });
        _thread_pool.addJob([this]() { waitDataLoop(); });

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
    Server::connectClient(std::string ip, const port_t port, connection_hndl_fn_t connect_hndl) {
        std::unique_ptr<Client> client(new Client(ConnectSocket(ip, port)) );
        connect_hndl(*client);
        _client_mutex.lock();
        _clients.emplace_back(std::move(client));
        _client_mutex.unlock();

        return true;
    }

    bool
    Server::disconnectClient(std::string ip, port_t port) {
        bool disconn = false;
        for (std::unique_ptr<Client>& client : _clients)
            if (client->getIP() == ip && client->getPort() == port) {
                client->disconnect();
                disconn = true;
            }
        return disconn;
    }

    void
    Server::disconnectAll() {
        for(std::unique_ptr<Client>& client : _clients) {
            /* client->disconnect(); */
        }
    }

    bool
    Server::sendToClient(std::string ip, port_t port, const std::string& data) {
        bool data_is_sended = false;
        for (std::unique_ptr<Client>& client : _clients)
            if (client->getIP() == ip && client->getPort() == port) {
                client->sendMessage(data);
                data_is_sended = true;
        }
        return data_is_sended;
    }

    void
    Server::sendToAll(const std::string& data) {
        for(std::unique_ptr<Client>& client : _clients)
            client->_protocol.sendMessage(client->_ip, data);
    }

    void
    Server::handleAcceptLoop() {
        if (_status == ServerStatus::running) {
            /* TODO AHTUNG!!! not clear memeory on error?!!!!!!!!!!!!!! */
            std::unique_ptr<Client> client( new Client(_socket.Accept()) );
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

    void
    Server::waitDataLoop()
    {
        std::lock_guard lock(_client_mutex);
        std::string data;

        /* for all clients */
        for (auto it = _clients.begin(), end = _clients.end(); it != end; ++it)
        {
            auto& client = *it;

            if (client) /* client reciving data */
            {
                if (client->recvMessage(data)) {
                    _thread_pool.addJob([this, _data = std::move(data), &client]() {
                        client->_access_mutex.lock();
                        _data_hndl_fn(_data, *client);
                        client->_access_mutex.unlock();
                    });
                }
            }
            else if (client->_status == ClientStatus::disconnected) /* on client disconnected */
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
