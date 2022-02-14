#ifndef SERVER_HPP_E2HLJCRX
#define SERVER_HPP_E2HLJCRX

#include <functional>
#include <mutex>
#include <string>
#include <map>
#include <list>

#include "INetDefs.hpp"
#include "Socket.hpp"
#include "Protocol.hpp"
#include <siigix/General/thread_pool.hpp>

namespace sgx {

    namespace Net {

        class ServerTCP {
            public:
                enum ServerStatus {
                    running,
                    shutdowned,
                };

                enum ClientStatus {
                    connected,
                    disconnected,
                };

                class Client  {
                    public:
                        friend class ServerTCP;

                        Client(TransSocket&& sock, ProtocolFactory*); //Protocol using in ServerXXX Class
                        virtual ~Client();

                        const SocketAddress& address() const { return _socket.address(); }
                        ClientStatus status() const { return _status;}
                        int disconnect();

                        void reciveMessage(std::string& data);
                        void sendMessage(const std::string& data);
                    private:
                        std::mutex   _access_mutex;
                        TransSocket  _socket;
                        Protocol     _protocol;
                        ClientStatus _status = ClientStatus::connected;
                };

                using connection_hndl_fn_t = std::function<void(Client&)>;
                using data_hndl_fn_t       = std::function<void(const std::string& data, Client&)>;

                typedef std::list<std::unique_ptr<Client>>::iterator ClientIterator;

            public:
                ServerTCP(std::string host = "127.0.0.1", port_t port = 8000, bool useip6 = false, bool useudp = false,
                        int max_conn = 10,
                        int threads = std::thread::hardware_concurrency());
                ServerTCP(data_hndl_fn_t d_hndl_fn,
                        connection_hndl_fn_t conn_hndl_fn,
                        connection_hndl_fn_t disconn_hdnl_fn,
                        std::string host = "127.0.0.1", port_t port = 8000, bool useip6 = false, bool useudp = false,
                        int max_conn = 10,
                        int threads = std::thread::hardware_concurrency());
                virtual ~ServerTCP();

                /* Setup & Run serve loop
                 * return ERRNO on fail or ServerTCP::UP on success */
                int start();

                /* Wait for end all threads in thread pool */
                void joinLoop();

                /* Stop serve loop */
                void stop();

                /* Handlers setters */
                void setDataHandler(data_hndl_fn_t handler)          { _data_hndl_fn    = handler; if (_status == running) start(); };
                void setConnHandler(connection_hndl_fn_t handler)    { _conn_hndl_fn    = handler; if (_status == running) start(); };
                void setDisconnHandler(connection_hndl_fn_t handler) { _disconn_hndl_fn = handler; if (_status == running) start(); }

                thread_pool& getThreadPool() {return _thread_pool;}

                bool connectClient(std::string ip, port_t port, connection_hndl_fn_t connect_hndl);
                bool connectClient(const SocketAddress& addr, connection_hndl_fn_t connect_hndl);

                void sendToAll(const std::string& message);

                bool sendToClient(std::string ip, port_t port, const std::string& load);
                bool sendToClient(const SocketAddress& addr, const std::string& load);

                bool disconnectClient(std::string ip, port_t port);
                bool disconnectClient(const SocketAddress& addr);

                void disconnectAll();

                const SocketAddress& address() const { return _socket.address(); }
            private:
                void waitDataLoop();
                void handleAcceptLoop();

                ListenSocket _socket;
                ProtocolFactory   *_usingProtocol;

                /* Event handlers */
                connection_hndl_fn_t _conn_hndl_fn    = [](Client&) {};
                connection_hndl_fn_t _disconn_hndl_fn = [](Client&) {};
                data_hndl_fn_t       _data_hndl_fn    = [](const std::string& data, Client&) {};

                thread_pool  _thread_pool;
                ServerStatus _status = ServerStatus::shutdowned;

                std::list<std::unique_ptr<Client>> _clients_list;
                std::mutex _clients_list_mutex;

        };

    } /* Net */ 

} /* sgx */ 

#endif /* end of include guard: SERVER_HPP_E2HLJCRX */
