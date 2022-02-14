#ifndef THREADPOOL_HPP_WEAY64UX
#define THREADPOOL_HPP_WEAY64UX

#include <thread>
#include <type_traits>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace sgx
{
    /* TODO add priority? */
    class ThreadPool {
        public:
            ThreadPool(size_t threads = std::thread::hardware_concurrency());
            virtual ~ThreadPool();

            void start(size_t threads);
            void stop();

            template<typename F>
            void addJob(F job)
            {
                if(_terminated) { return; }

                {
                    std::unique_lock lock(_queue_mutex);
                    _worker_queue.push(job);
                }

               _condition.notify_one();
            }

            template<typename F, typename... Arg>
            void addJob(const F& job, const Arg&... args) { addJob( [job, args...]{ job(args...); } ); }

            void join();
            void dropJobs();

            size_t threads();
        private:
            void worker();
            void setup(unsigned int threads);

            std::vector<std::thread>          _threads;
            std::queue<std::function<void()>> _worker_queue;
            std::mutex                        _queue_mutex;
            std::atomic<bool>                 _terminated;
            std::condition_variable           _condition;
    };
} /* sgx */ 

#endif /* end of include guard: THREADPOOL_HPP_WEAY64UX */
