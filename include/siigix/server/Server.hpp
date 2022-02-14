#ifndef SERVER_HPP_E2HLJCRX
#define SERVER_HPP_E2HLJCRX

#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <functional>
#include <mutex>
#include <string>
#include <map>
#include <list>

#include <general/Socket.hpp>
#include <general/Protocol.hpp>
#include <general/ThreadPool.hpp>
/* #include <general/INData.hpp> */
#include <general/Status.hpp>

namespace siigix {

    namespace TCP {

        template<class ProtocolC>
        class Server {
            public:
                enum ServerStatus {
                    running,
                    shutdowned,
                };

                enum ClientStatus {
                    connected,
                    disconnected,
                };

                template<class CliProtocolC>
                class Client  {
                    public:
                        friend class Server;

                        Client(TransSocket&& sock);
                        virtual ~Client();

                        std::string  getIP() const     { return _socket.getIP(); }
                        port_t       getPort() const   { return _socket.getPort(); }
                        ClientStatus getStatus() const { return _status;}
                        int disconnect();

                        void recvMessage(std::string& data);
                        void sendMessage(const std::string& data);
                    private:
                        std::mutex    _access_mutex;
                        TransSocket   _socket;
                        CliProtocolC  _proto;
                        ClientStatus  _status = ClientStatus::connected;
                };

                using connection_hndl_fn_t = std::function<void(Client<ProtocolC>&)>;
                using data_hndl_fn_t       = std::function<void(const std::string& data, Client<ProtocolC>&)>;

                /* template<class ProtocolC> */
                /* typedef std::list<std::unique_ptr<Client<ProtocolC>>>::iterator ClientIterator; */

            public:
                Server(std::string host = "127.0.0.1", port_t port = 8000, bool useip6 = false, bool useudp = false,
                        int max_conn = 10,
                        int threads = std::thread::hardware_concurrency());
                Server(data_hndl_fn_t d_hndl_fn,
                        connection_hndl_fn_t conn_hndl_fn,
                        connection_hndl_fn_t disconn_hdnl_fn,
                        std::string host = "127.0.0.1", port_t port = 8000, bool useip6 = false, bool useudp = false,
                        int max_conn = 10,
                        int threads = std::thread::hardware_concurrency());
                virtual ~Server();

                /* Setup & Run serve loop
                 * return ERRNO on fail or Server::UP on success */
                int start();

                /* Wait for end all threads in thread pool */
                void joinLoop();

                /* Stop serve loop */
                void stop();

                /* Handlers setters */
                void setDataHandler(data_hndl_fn_t handler)          { _data_hndl_fn    = handler; if (_status == running) start(); };
                void setConnHandler(connection_hndl_fn_t handler)    { _conn_hndl_fn    = handler; if (_status == running) start(); };
                void setDisconnHandler(connection_hndl_fn_t handler) { _disconn_hndl_fn = handler; if (_status == running) start(); }

                ThreadPool& getThreadPool() {return _thread_pool;}

                /* Add new client on address ip and port port, and after successfuly connection run connect_hndl funciton
                 * return bool of success */
                bool connectClient(std::string ip, port_t port, connection_hndl_fn_t connect_hndl);

                /* Send data from INData load to all connected clients */
                void sendToAll(const std::string& load);

                /* Send data from INData load to connected client on address ip and port port
                 * return bool of success */
                bool sendToClient(std::string ip, port_t port, const std::string& load);

                /* Disconnect client with IP and port / disconnect client by its object
                 * return bool of success */
                bool disconnectClient(std::string ip, port_t port);
                /* int disconnectClient(Client& client); */

                /* Disconnect all connected clients */
                void disconnectAll();
            private:
                void waitDataLoop();
                void handleAcceptLoop();

                ListenSocket _socket;

                /* Event handlers */
                connection_hndl_fn_t _conn_hndl_fn    = [](Client<ProtocolC>&) {};
                connection_hndl_fn_t _disconn_hndl_fn = [](Client<ProtocolC>&) {};
                data_hndl_fn_t       _data_hndl_fn    = [](const std::string& data, Client<ProtocolC>&) {};

                ThreadPool      _thread_pool;
                ServerStatus    _status = ServerStatus::shutdowned;

                std::list<std::unique_ptr<Client<ProtocolC>>> _clients;
                std::mutex _client_mutex;

        };

        #include "Server.inl"
        #include "ServerClient.inl"

        typedef Server<TCP::Protocol> ServerTCP;
        typedef ServerTCP::Client<TCP::Protocol> ServerClientTCP;

    } /* TCP */ 

} /* siigiix */ 

#endif /* end of include guard: SERVER_HPP_E2HLJCRX */
