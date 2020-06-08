#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <type_traits>
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool
{
public:
    ThreadPool(size_t = std::thread::hardware_concurrency() * 4);

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<std::result_of_t<F(Args...)>>;

    ~ThreadPool();

private:

    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queue_mutex;
    std::condition_variable condition;
    std::atomic_bool stop;
};


template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> std::future<std::result_of_t<F(Args...)>>
{
    using return_type = std::result_of_t<F(Args...)>;

    auto task = std::make_shared< std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        if(stop)
            throw std::runtime_error("already stopped");

        tasks.emplace([task](){ (*task)(); });
    }

    condition.notify_one();
    return res;
}

#endif
