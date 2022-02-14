#include <Server.hpp>
#include <Protocol.hpp>
#include <stdexcept>
#include <thread>

namespace sgx {
namespace Net {

    ServerTCP::ServerTCP(std::string host, port_t port, bool useip6, bool useudp, int max_conn, int threads) :
        ServerTCP(
                [](const std::string& data, Client&) {},
                [](Client&) {},
                [](Client&) {},
                host, port, useip6, useudp,
                max_conn, threads)
    { }

    ServerTCP::ServerTCP(
            data_hndl_fn_t d_hndl_fn,
            connection_hndl_fn_t conn_hndl_fn,
            connection_hndl_fn_t disconn_hdnl_fn,
            std::string host, port_t port, bool useip6, bool useudp,
            int max_conn, int threads) :
        _socket(SocketAddress(host, port), {
                    SocketOption(SOL_SOCKET, SO_REUSEADDR, USE_OPT_FLAG, sizeof(USE_OPT_FLAG)),
                    SocketOption(SOL_SOCKET, SO_REUSEPORT, USE_OPT_FLAG, sizeof(USE_OPT_FLAG))
                },
                max_conn),
        _data_hndl_fn(d_hndl_fn),
        _conn_hndl_fn(conn_hndl_fn),
        _disconn_hndl_fn(disconn_hdnl_fn),
        _thread_pool(threads),
        _usingProtocol(new ProtocolSimpleFactory()) //TODO ITS TEMP!
    {
        _thread_pool.sleep_duration = 100000;
    }

    ServerTCP::~ServerTCP()
    {
        if (_status == ServerStatus::running) {
            stop();
        }
    }

    int
    ServerTCP::start()
    {
        if (_status == ServerStatus::running) {
            stop();
        }

        _status = ServerStatus::running;

        if (!_socket.isValid()) {
            _status = ServerStatus::shutdowned;
            throw std::domain_error(eprintf("ServerTCP::", __func__, ": _socket is invalid"));
        }

        _thread_pool.push_task([this]() { handleAcceptLoop(); });
        _thread_pool.push_task([this]() { waitDataLoop(); });

        return _status;
    }

    void
    ServerTCP::stop()
    {
        _thread_pool.wait_for_tasks();
        _status = ServerStatus::shutdowned;
        _socket.Close();
        _clients_list.clear();
    }

    void ServerTCP::joinLoop() {_thread_pool.wait_for_tasks();}

    bool
    ServerTCP::connectClient(std::string ip, const port_t port, connection_hndl_fn_t connect_hndl) {
        std::unique_ptr<Client> client(new Client(ConnectSocket(ip, port), _usingProtocol) );
        connect_hndl(*client);
        _clients_list_mutex.lock();
        _clients_list.emplace_back(std::move(client));
        _clients_list_mutex.unlock();

        return true;
    }

    bool
    ServerTCP::disconnectClient(std::string ip, port_t port) {
        bool disconn = false;
        for (std::unique_ptr<Client>& client : _clients_list)
            if (client->address() == SocketAddress(ip, port)) {
                client->disconnect();
                disconn = true;
            }
        return disconn;
    }

    void
    ServerTCP::disconnectAll() {
        for(std::unique_ptr<Client>& client : _clients_list) {
            client->disconnect();
        }
    }

    bool
    ServerTCP::sendToClient(std::string ip, port_t port, const std::string& data) {
        bool data_is_sended = false;
        for (std::unique_ptr<Client>& client : _clients_list)
            if (client->address() == SocketAddress(ip, port)) {
                client->sendMessage(data);
                data_is_sended = true;
        }
        return data_is_sended;
    }

    void
    ServerTCP::sendToAll(const std::string& data) {
        for(std::unique_ptr<Client>& client : _clients_list) {
            client->sendMessage(data);
        }
    }

    void
    ServerTCP::handleAcceptLoop() {
        if (_status == ServerStatus::running) {
            /* std::unique_ptr<Client> client( new Client(_socket.Accept(), _usingProtocol) ); */
            std::unique_ptr<Client> client( new Client(_socket.Accept(SOCK_NONBLOCK), _usingProtocol) );
            _conn_hndl_fn(*client);
            _clients_list_mutex.lock();
            _clients_list.emplace_back(std::move(client));
            _clients_list_mutex.unlock();
        }

        //loop
        if (_status == ServerStatus::running) {
            _thread_pool.push_task([this](){handleAcceptLoop();});
        }
    }

    void
    ServerTCP::waitDataLoop()
    {
        {
            std::lock_guard lock(_clients_list_mutex);

            /* for all clients */
            for (auto it = _clients_list.begin(), end = _clients_list.end(); it != end; ++it)
            {
                auto& cur_client = *it;

                if (cur_client->status() == ClientStatus::connected) /* client reciving data */
                {
                    std::string data;
                    cur_client->reciveMessage(data);
                    if (data.length() > 0)
                    {
                        _thread_pool.push_task(
                            [this, _data = std::move(data), &cur_client]()
                            {
                                std::lock_guard lock(cur_client->_access_mutex);
                                _data_hndl_fn(_data, *cur_client);
                            }
                        );
                    }
                } /* on reciving data END */
                else if (cur_client->_status == ClientStatus::disconnected) /* on client disconnected */
                {
                    _thread_pool.push_task(
                        [this, &cur_client, it]()
                        {
                            if (!cur_client)
                                return;

                            {
                                std::lock_guard lock(cur_client->_access_mutex);
                                _disconn_hndl_fn(*cur_client);
                            }
                            _clients_list.erase(it);
                            /* delete cur_client.release(); */
                        }
                    );
                } /* on client disconnected END */
            } /* for all clients END */
        } /* lock_guard END */

        //loop
        if (_status == ServerStatus::running) {
            _thread_pool.push_task([this](){waitDataLoop();});
        }
    }

} /* Net */ 
} /* sgx  */ 
