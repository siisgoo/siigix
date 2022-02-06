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
#include <general/ThreadPool.hpp>
#include <general/IOBuff.hpp>
#include <general/Status.hpp>

namespace siigix {

namespace TCP {

    struct Server {
        public:
            struct Client {
                public:
                    friend struct Server;

                    Client(INet::Socket socket, INet::sockaddr_in_any address);
                    virtual ~Client();

                    INet::sockaddr_in_any getAddr() const;
                    ClientStatus getStatus() const {return _status;}
                    ClientStatus disconnect();

                    IOBuff recive();
                    bool send(const IOBuff& data) const;
                private:
                    std::mutex _access_mutex;
                    INet::sockaddr_in_any _address;

                    INet::Socket _cli_socket;
                    ClientStatus _status = ClientStatus::connected;
            };

            typedef int ka_prop_t;
            struct KeepAliveConfig{
                ka_prop_t ka_idle  = 120;
                ka_prop_t ka_intvl = 3;
                ka_prop_t ka_cnt   = 5;
            };

            typedef std::function<void(Client&)> connection_hndl_fn_t;
            typedef std::function<void(const IOBuff&, Client&)> data_hndl_fn_t;
            static constexpr auto default_conn_hndl_fn = [](Client&){};
            static constexpr auto default_data_hndl_fn = [](const IOBuff&, Client&){};
            typedef std::list<std::unique_ptr<Client>>::iterator ClientIterator;

        public:
            Server(data_hndl_fn_t dfn,
                    bool useip6 = false, int port = 8000, std::string ip = "0.0.0.0",
                    int threads = std::thread::hardware_concurrency());
            Server(data_hndl_fn_t d_hndl_fn,
                    connection_hndl_fn_t conn_hndl_fn,
                    connection_hndl_fn_t disconn_hdnl_fn,
                    bool useip6 = false, int port = 8000, std::string ip = "0.0.0.0",
                    int threads = std::thread::hardware_concurrency());
            /* Server(struct in_addr6 ip6); */
            virtual ~Server();

            ServerStatus start();
            void joinLoop();
            void stop();

            void setHandler(data_hndl_fn_t handler);

            ThreadPool& getThreadPool() {return _thread_pool;}

            bool connectTo(const INet::sockaddr_in_any to, connection_hndl_fn_t connect_hndl);
            void send(const IOBuff&);
            bool sendTo(const INet::sockaddr_in_any to, const IOBuff& load);
            bool disconnectHost(const INet::sockaddr_in_any host);
            void disconnectAll();

            void setKeepAlive(KeepAliveConfig nkac) { _ka_conf = nkac; }
        private:
            bool enableKeepAlive(INet::Socket socket);
            void waitDataLoop();
            void handleAcceptLoop();

            INet::Socket _socket;
            ////////////////////////ADD IP6 KARSIVO/////////////////////////
            INet::sockaddr_in_any _server_address; //TODO o_O

            connection_hndl_fn_t _conn_hndl_fn    = default_conn_hndl_fn;
            connection_hndl_fn_t _disconn_hndl_fn = default_conn_hndl_fn;
            data_hndl_fn_t       _data_hndl_fn    = default_data_hndl_fn;

            ThreadPool      _thread_pool;
            KeepAliveConfig _ka_conf;
            ServerStatus    _status;

            std::list<std::unique_ptr<Client>> _clients;
            std::mutex _client_mutex;
    };

} /* TCP */ 

} /* siigiix */ 

#endif /* end of include guard: SERVER_HPP_E2HLJCRX */
