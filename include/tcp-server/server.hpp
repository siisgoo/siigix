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

#include "socket.hpp"
#include "threadPool.hpp"
#include "iobuff.hpp"
#include "types.hpp"
#include "status.hpp"

using namespace INet;

namespace TCP {

    std::map<std::string, in_addr_t> getLocalIP();
    bool operator==(struct sockaddr_in, struct sockaddr_in);

    struct Server {
        public:
            struct Client {
                public:
                    friend struct Server;

                    Client(Socket socket, struct sockaddr_in address);
                    virtual ~Client();

                    struct sockaddr_in getAddr() const;
                    /* virtual struct sockaddr_in6 getAddrV6() const; */
                    ClientStatus getStatus() const {return _status;}
                    ClientStatus disconnect();

                    IOBuff recive();
                    bool send(const IOBuff& data) const;
                private:
                    std::mutex _access_mutex;
                    union sockaddr_any _address;
                    bool _use_ip6;

                    Socket _cli_socket;
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

            bool connectTo(const struct sockaddr_in to, connection_hndl_fn_t connect_hndl);
            /* bool connectTo(struct in6_sockaddr to, connection_hndl_fn_t connect_hndl); */
            void send(const IOBuff&);
            bool sendTo(const struct sockaddr_in to, const IOBuff& load);
            /* bool sendTo(struct in6_sockaddr to, const IOBuff& load); */
            bool disconnectHost(const struct sockaddr_in host);
            /* bool disconnectHost(struct in6_sockaddr host); */
            void disconnectAll();

            void setKeepAlive(KeepAliveConfig nkac) { _ka_conf = nkac; }
        private:
            bool enableKeepAlive(Socket socket);
            void waitDataLoop();
            void handleAcceptLoop();

            Socket _socket;
            ////////////////////////ADD IP6 KARSIVO/////////////////////////
            __uint16_t _port;
            in_addr_t _ip;
            union sockaddr_any _server_address; //TODO o_O
            bool _use_ip6 = false;

            connection_hndl_fn_t _conn_hndl_fn    = default_conn_hndl_fn;
            connection_hndl_fn_t _disconn_hndl_fn = default_conn_hndl_fn;
            data_hndl_fn_t       _data_hndl_fn    = default_data_hndl_fn;

            ThreadPool      _thread_pool;
            KeepAliveConfig _ka_conf;
            ServerStatus    _status;

            std::list<std::unique_ptr<Client>> _clients;
            std::mutex _client_mutex;
    };

} /* TCP {  */ 

#endif /* end of include guard: SERVER_HPP_E2HLJCRX */
