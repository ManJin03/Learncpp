//
// Created by 33550 on 2026/10/17.
//
#include <functional>
#include <future>
#include <iostream>
#include <queue>
#include <thread>
#include <vector>

namespace ThreadPool
{
    template <typename T>
    class thread_safe_queue
    {
        std::queue<T> m_queue;
        std::mutex m_mutex;
    };

    class Thread_pool
    {
        using ThreadVectorType = std::vector<std::thread>;
        using TaskQueueType = std::queue<std::move_only_function<void()>>;
        const int QUEUE_SIZE{16};
        const size_t thread_num{4};

    public:
        template <typename Callable>
        std::future<std::invoke_result_t<Callable()>> pushtask(Callable&& call)
        {
            using result_type = std::invoke_result_t<Callable()>;
            std::packaged_task<result_type()> task{call};
            auto f = task.get_future();
            std::unique_lock<std::mutex> lock{m_queue_mutex};
            while (m_queue.size() >= QUEUE_SIZE) {
                lock.unlock();
                std::this_thread::yield();
                lock.lock();
            }
            m_queue.push(std::move(task));
            lock.unlock();
            m_queue_cond.notify_all();
            return f;
        }

        Thread_pool()
        {
            try {
                for (int i = 0 ; i < thread_num ; ++i) {
                    m_threads.emplace_back(&Thread_pool::workloop , this , i);
                }
            } catch (...) {
                m_running.store(false);
                throw;
            }
        }

        ~Thread_pool()
        {
            m_running.store(false);
            m_queue_cond.notify_all();
            for (auto& t : m_threads) {
                if (t.joinable()) {
                    t.join();
                }
            }
        }

    private:
        void workloop(const int id)
        {
            printf("%u start\n" , id);
            std::unique_lock<std::mutex> lock{m_queue_mutex};
            while (m_running.load()) {
                if (m_queue.empty()) {
                    m_queue_cond.wait(lock , [this] { return !m_running || !m_queue.empty(); });
                    if (!m_running.load()) {
                        break;
                    }
                }
                auto task = std::move(m_queue.front());
                m_queue.pop();
                lock.unlock();
                printf("%d run task:\n" , id);
                task();
                lock.lock();
            }
            printf("%u end\n" , id);
        }

    private:
        ThreadVectorType m_threads;
        std::mutex m_queue_mutex;
        std::condition_variable m_queue_cond;
        TaskQueueType m_queue;
        std::atomic<bool> m_running{true};
    };
}

int sleep1s(const int i)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    printf("%d:sleep1s\n" , i);
    return i;
}

int main() {}
