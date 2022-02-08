#ifndef CLIENT_HPP_3GEMBU54
#define CLIENT_HPP_3GEMBU54

#include <mutex>
#include <functional>

#include <netinet/in.h>

#include "threadPool.hpp"
#include "types.hpp"
#include "socket.hpp"
#include "status.hpp"
#include "iobuff.hpp"

using namespace INet;

namespace TCP {
    class Client {
        typedef std::function<void(INData&)> client_data_hndl_fn_t;

        enum ThreadManagementType : bool {
            single_thread = false,
            thread_pool = true
        };

        union Thread {
            std::thread* thread;
            ThreadPool* thread_pool;
            Thread() : thread(nullptr) {}
            Thread(ThreadPool* thread_pool) : thread_pool(thread_pool) {}
            ~Thread() {}
        };

        public:
            Client();
            Client(ThreadPool *thread_pool);
            virtual ~Client();

            ClientStatus connect(struct addrinfo conn_to, inet_port_t port);
            ClientStatus disconnected();

            bool send(INData& data);
            INData recive();
            INData reciveSync();

            /* void setHandler(data_hndl_fn_t nfn); */
            void joinHandler();

            struct sockaddr_in getHost() { return _address; }
            inet_port_t getPort() { return _address.sin_port; }
            ClientStatus status() { return _status; }
            int state()           { return _status.state; }
        private:
            ThreadManagementType thread_management_type;
            Thread threads;
            std::mutex _mutex;
            client_data_hndl_fn_t handler_func = [](INData&){};
            ClientStatus _status = ClientStatus::disconnected;
            Socket _socket;
            struct sockaddr_in _address;
    };
} /* TCP */ 

#endif /* end of include guard: CLIENT_HPP_3GEMBU54 */
