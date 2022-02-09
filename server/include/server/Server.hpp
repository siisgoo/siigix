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

    /* template<class ProtoC> */
    struct Server {
        public:
            enum ServerStatus {
                running,
                shutdowned,
            };

            enum ClientStatus {
                connected,
                disconnected,
            };

            struct Client  {
                public:
                    friend struct Server;

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
                    TCP::Protocol _proto;
                    ClientStatus  _status = ClientStatus::connected;
            };

            typedef std::function<void(Client&)>                connection_hndl_fn_t;
            typedef std::function<void(const std::string& data, Client&)> data_hndl_fn_t;

            static constexpr auto default_conn_hndl_fn = [](Client&){};
            static constexpr auto default_data_hndl_fn = [](const std::string& data, Client&){};

            typedef std::list<std::unique_ptr<Client>>::iterator ClientIterator;

        public:
            Server(port_t port = 8000, int max_conn = 10,
                    int threads = std::thread::hardware_concurrency());
            Server(data_hndl_fn_t d_hndl_fn,
                    connection_hndl_fn_t conn_hndl_fn,
                    connection_hndl_fn_t disconn_hdnl_fn,
                    port_t port = 8000, int max_conn = 10,
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
            void setDataHandler(data_hndl_fn_t handler)          { _data_hndl_fn = handler;    if (_status == running) start(); };
            void setConnHandler(connection_hndl_fn_t handler)    { _conn_hndl_fn = handler;    if (_status == running) start(); };
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
            connection_hndl_fn_t _conn_hndl_fn    = default_conn_hndl_fn;
            connection_hndl_fn_t _disconn_hndl_fn = default_conn_hndl_fn;
            data_hndl_fn_t       _data_hndl_fn    = default_data_hndl_fn;

            ThreadPool      _thread_pool;
            ServerStatus    _status = ServerStatus::shutdowned;

            std::list<std::unique_ptr<Client>> _clients;
            std::mutex _client_mutex;
    };

} /* TCP */ 

} /* siigiix */ 

#endif /* end of include guard: SERVER_HPP_E2HLJCRX */
