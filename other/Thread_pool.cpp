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
    class Thread_pool
    {
        using ThreadVectorType = std::vector<std::thread>;
        using TaskQueueType = std::queue<std::move_only_function<void()>>;
        using TaskQueueCondType = std::condition_variable;
        const size_t QUEUE_SIZE{16};
        const size_t thread_num{4};

    public:
        template <typename Callable , typename... Args>
        auto submit(Callable&& callable , Args&&... args)
            -> std::future<std::invoke_result_t<Callable,Args...>>
        {
            using result_type = std::invoke_result_t<Callable,Args...>;
            std::packaged_task<result_type()>
                    task([callable=std::forward<Callable>(callable)
                            ,...args=std::forward<Args>(args)]()mutable-> result_type
                        {
                            return std::invoke(std::move(callable) , std::move(args)...);
                        });
            auto f = task.get_future();
            std::unique_lock<std::mutex> lock{m_queue_mutex};
            while (m_queue.size() >= QUEUE_SIZE) {
                m_queue_full.wait(lock);
            }
            m_queue.push(std::move(task));
            lock.unlock();
            m_queue_empty.notify_one();
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

        ~Thread_pool() { shutdown(); }

    private:
        void workloop(const int id)
        {
            printf("%u start\n" , id);
            while (true) {
                std::unique_lock<std::mutex> lock{m_queue_mutex};
                if (m_running && m_queue.empty()) {
                    m_queue_empty.wait(lock , [this] { return !m_running || !m_queue.empty(); });
                }
                if (!m_running && (m_queue.empty() || std::chrono::steady_clock::now() >= m_deadline)) {
                    break;
                }
                auto task = std::move(m_queue.front());
                m_queue.pop();
                lock.unlock();
                m_queue_full.notify_one();
                printf("%d run task:\n" , id);
                task();
            }
            printf("%u end\n" , id);
        }

        void shutdown()
        {
            m_deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
            m_running = false;
            m_queue_empty.notify_all();
            for (auto& t : m_threads) {
                if (t.joinable()) {
                    t.join();
                }
            }
        }

    private:
        ThreadVectorType m_threads;
        TaskQueueCondType m_queue_empty;
        TaskQueueCondType m_queue_full;
        std::mutex m_queue_mutex;
        TaskQueueType m_queue;
        std::atomic_bool m_running{true};
        std::chrono::steady_clock::time_point m_deadline;
    };
}


int count;

void sleep1()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    printf("sleep1s\n");
    ++count;
}

int sleep1s(const int i)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    printf("%d:sleep1s\n" , i);
    return i;
}

int usleep(std::unique_ptr<int> p)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    printf("%d:usleep\n" , *p);
    return *p;
}

int main()
{
    ThreadPool::Thread_pool pool;
    std::future<int> future[20];
    for (int i = 0 ; i < 20 ; ++i) {
        future[i] = pool.submit(sleep1s , i);
    }
    for (auto& i : future) {
        std::cout << i.get();
    }
    std::cout << '\n';
    auto u = std::make_unique<int>(50);
    pool.submit(usleep , std::move(u));
    // int end;
    // std::cin >> end;
}
