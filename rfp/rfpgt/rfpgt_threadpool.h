#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <dplp_promise.h>

#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

using namespace std;

class ThreadPool {
  public:
    ThreadPool(size_t threads);

    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        -> dplp::Promise<typename std::result_of<F(Args...)>::type>;

    ~ThreadPool();

  private:
    std::vector<std::thread>           m_workers;  // thread queue
    std::queue<std::function<void()> > m_jobs;     // job queue
    std::mutex              m_queue_mutex;         // mutex for the job queue
    std::condition_variable m_cv;  // conditional variable for this thread pool
    bool                    m_stop;  // put thread pool in cancellation state
};

inline
ThreadPool::ThreadPool(size_t threads)
: m_stop(false)
{
    for (auto i = 0; i < threads; i++) {
        m_workers.emplace_back([this] {
            for (;;) {
                std::function<void()> job;
                {
                    std::unique_lock<std::mutex> lock(this->m_queue_mutex);
                    this->m_cv.wait(lock, [this] {
                        return this->m_stop || !this->m_jobs.empty();
                    });
                    if (this->m_stop && this->m_jobs.empty()) {
                        return;
                    }
                    job = std::move(this->m_jobs.front());
                    this->m_jobs.pop();
                }
                job();
            }
        });
    }
}

template <class F, class... Args>
auto ThreadPool::enqueue(
    F&&       f,
    Args&&... args) -> dplp::Promise<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

    return dplp::Promise<return_type>([&](auto fulfill, auto reject) {
        {
            std::unique_lock<std::mutex> lock(m_queue_mutex);
            if (m_stop) {
                reject(std::make_exception_ptr(std::runtime_error("Boo!")));
            }
            m_jobs.emplace([ =]() { fulfill(func()); });
        }
        m_cv.notify_one();
    });
}

// the destructor joins all threads
inline
ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        m_stop = true;
    }

    m_cv.notify_all();

    for (std::thread& worker : m_workers)
        worker.join();
}

#endif
