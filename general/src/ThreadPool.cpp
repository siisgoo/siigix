#include <ThreadPool.hpp>

using namespace siigix;

ThreadPool::ThreadPool(size_t threads)
{
    setup(threads);
}

ThreadPool::~ThreadPool()
{
    _terminated = true;
    join();
}

void
ThreadPool::start(unsigned int threads) {
    if (!_terminated) {
        return;
    }
    _terminated = false;
    setup(threads);
}

void
ThreadPool::stop() { _terminated = true; join(); }

/* template<typename F> */
void
ThreadPool::addJob(std::function<void()> job) {
    if (_terminated) {
        return;
    }

    {
        std::unique_lock lock(_queue_mutex);
        _job_queue.push(job);
    }
    _condition.notify_one();
}

/* template<typename F, typename... Args> */
/* void */
/* ThreadPool::addJob(const F& job, const Args&... args) { */
/*     addJob([job, args...] () { */
/*         job(args...); */
/*     }); */
/* } */

void ThreadPool::join() { for (auto& t: _threads) t.join(); }

void
ThreadPool::dropJobs() {
    _terminated = true;
    join();
    _terminated = false;
    std::queue<std::function<void()>> empty;
    std::swap(_job_queue, empty);
    // reset thread pool
    setup(_threads.size());
}

size_t
ThreadPool::threads() { return _threads.size(); }

void
ThreadPool::worker() {
    std::function<void()> job;
    while (!_terminated) {
        {
            std::unique_lock lock(_queue_mutex);
            _condition.wait(lock, [this](){return !_job_queue.empty() || _terminated;});
            if(_terminated) return; //exit
            job = _job_queue.front(); //run
            _job_queue.pop();
        }
        job();
    }
}

void
ThreadPool::setup(unsigned int threads) {
    _threads.clear();
    for (unsigned int i = 0; i < threads; i++) {
        _threads.emplace_back(&ThreadPool::worker, this);
    }
}
