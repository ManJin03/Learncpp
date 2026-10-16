//
// Created by 33550 on 2026/10/15.
//
#include <iostream>
#include <atomic>
#include <memory>
#include <thread>
#include <unistd.h>
#include <vector>
using std::cout;

namespace t1
{
    class spinlock_mutex
    {
        std::atomic_flag m_flag{};

    public:
        void lock()
        {
            while (m_flag.test_and_set(std::memory_order_acquire));
        }

        void unlock()
        {
            m_flag.clear();
        }
    };

    void run()
    {
        std::atomic<bool> b{true};
        bool x = b.load(std::memory_order_relaxed);
        cout << x << '\n';
        b.store(false);
        x = b.exchange(true , std::memory_order_acq_rel);
        cout << x << '\n';
        cout << b.load(std::memory_order_relaxed) << '\n';
        bool expected = false;
        while (!b.compare_exchange_weak(expected , true)) {
            cout << b.load() << '\n';
        }
        cout << expected << '\n';
        int arr[5] = {0 , 1 , 2 , 3 , 4};
        std::atomic<int*> p(arr);
        int* y = p.fetch_add(2);
        cout << *y << '\n';
        cout << *p.load() << '\n';
        cout << *(p -= 1) << '\n';
        std::shared_ptr<int> q = std::make_shared<int>(10);
        std::shared_ptr<int> sptr = std::atomic_load(&q);
        cout << *sptr << '\n';
        *sptr = 15;
        std::atomic_store(&q , sptr);
        cout << *std::atomic_load(&q) << '\n';
    }
}

namespace t2
{
    std::atomic<bool> x{false} , y{false};
    std::atomic<int> z{0};

    void clear()
    {
        x.store(false);
        y.store(false);
        z.store(0);
    }

    void w_x()
    {
        x.store(true , std::memory_order_seq_cst);
    }

    void w_y()
    {
        y.store(true , std::memory_order_seq_cst);
    }

    void r_x_t_y()
    {
        while (!x.load(std::memory_order_seq_cst)) {}
        if (y.load(std::memory_order_seq_cst)) {
            ++z;
        }
    }

    void r_y_t_x()
    {
        while (!y.load(std::memory_order_seq_cst)) {}
        if (x.load(std::memory_order_seq_cst)) {
            ++z;
        }
    }

    void run1()
    {
        std::thread t1(w_x);
        std::thread t2(w_y);
        std::thread t3(r_x_t_y);
        std::thread t4(r_y_t_x);
        t1.join();
        t2.join();
        t3.join();
        t4.join();
        cout << z << '\n';
    }

    void w_x_t_y()
    {
        x.store(true , std::memory_order_relaxed);
        y.store(true , std::memory_order_relaxed);
    }

    void r_y_t_x_()
    {
        while (!y.load(std::memory_order_relaxed)) {}
        if (x.load(std::memory_order_relaxed)) {
            ++z;
        }
    }

    void run2()
    {
        std::thread t1(w_x_t_y);
        std::thread t2(r_y_t_x_);
        t1.join();
        t2.join();
        cout << z << '\n';
    }

    void thread1()
    {
        cout << "first\n";
        z.store(1 , std::memory_order_release);
    }

    void thread2()
    {
        int t = 1;
        while (!z.compare_exchange_weak(t , 2 , std::memory_order_acq_rel)) {
            t = 1;
        }
        cout << "second\n";
    }

    void thread3()
    {
        while (z.load(std::memory_order_acquire) < 2) {}
        cout << "thrid\n";
    }

    void run3()
    {
        std::thread t1(thread1);
        std::thread t2(thread2);
        std::thread t3(thread3);
        t1.join();
        t2.join();
        t3.join();
        cout << z << '\n';
    }

    void run()
    {
        run1();
        clear();
        run2();
        clear();
        run3();
    }
}

namespace t3
{
    std::vector<int> data{};
    std::atomic<int> count{0};
    std::atomic<bool> end{false};

    void prepare()
    {
        for (int i = 0 ; i < 100 ; ++i) {
            data.push_back(i);
        }
        count.store(100);
    }

    void produce()
    {
        while (!end.load(std::memory_order_relaxed)) {
            int index{};
            if (index = count.fetch_sub(1 , std::memory_order_relaxed) ; index <= 0) {
                std::this_thread::yield();
                continue;
            }
            cout << data[index] << '\n';
        }
    }

    std::atomic<bool> x{} , y{};
    std::atomic<int> z{};

    void w_x_t_y()
    {
        x.store(true , std::memory_order_relaxed);
        std::atomic_signal_fence(std::memory_order_release);
        y.store(true , std::memory_order_relaxed);
    }

    void r_y_t_x()
    {
        while (!y.load(std::memory_order_relaxed)) {}
        std::atomic_signal_fence(std::memory_order_acquire);
        if (x.load(std::memory_order_relaxed)) {
            ++z;
        }
    }

    void run()
    {
        std::thread t1(prepare);
        std::thread t2(produce);
        std::thread t3(produce);
        t1.join();
        t2.detach();
        t3.detach();
        sleep(1);
        end.store(true);
        cout << count.load() << '\n';
        std::thread t4(w_x_t_y);
        std::thread t5(r_y_t_x);
        t4.join();
        t5.join();
        cout << z << '\n';
    }
}

namespace t4 {}

int main()
{
    t3::run();
}
