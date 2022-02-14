#ifndef CLIENT_HPP_Q2HAEQTG
#define CLIENT_HPP_Q2HAEQTG

#include <string>

#include <ThreadPool.hpp>
#include "Socket.hpp"
#include "INetDefs.hpp"
#include <type_traits>

namespace sgx {
namespace Net {

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

    enum status {
        connected,
        disconnect,
    };

    /* class Client : public IClient { */
    class Client {
        typedef std::function<void(const std::string&)> data_hndl_fn_t;

        public:
            Client();
            Client(ThreadPool *thread_pool);
            virtual ~Client();

            void handleSingleThread();
            void handleThreadPool();

            bool connectTo(std::string ip, port_t port);
            bool disconnect();

            bool sendMessage(const std::string& data);
            bool reciveMessage(std::string& data);
            bool reciveMessageSync(std::string& data);

            void setHandler(data_hndl_fn_t nfn);
            void joinHandler();

            std::string getIP() const { return _ip; }
            port_t getPort() const    { return _port; }
            status getStatus() const  { return _status; }
        private:
            ThreadManagementType thread_management_type;
            Thread threads;
            std::mutex _mutex;
            status _status;
            data_hndl_fn_t handler_func = [](const std::string&){};

            std::string _ip;
            port_t _port;
            ConnectSocket _socket;
    };
} /* Net */ 
} /* sgx */ 

#endif /* end of include guard: CLIENT_HPP_Q2HAEQTG */
