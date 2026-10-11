//
// Created by 33550 on 2026/10/8.
//
#include <algorithm>
#include <iostream>
#include <memory>
#include <numeric>
#include <thread>
#include <vector>
#include <chrono>
#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <unistd.h>
using std::cout;

namespace t1
{
    class Hello
    {
        int value{};

    public:
        void hello(const int i) const
        {
            cout << "hello from " << value + i << std::endl;
        }

        Hello() = default;

        explicit Hello(int i) : value(i) {};
    };

    void hello(int id)
    {
        cout << "hello thread" << id << '\n';
    }

    void runthread(std::thread t)
    {
        cout << "In runthread";
        t.join();
    }

    void run()
    {
        std::thread t1(hello , 1);
        t1.detach();
        std::thread t2([]
        {
            cout << "hello thread2" << std::endl;
        });
        t2.join();
        Hello tem{10};
        std::thread t3(&Hello::hello , &tem , 1);
        t3.join();
        int value = 10;
        std::thread t4([] (int& i) { i += 15; } , std::ref(value));
        t4.join();
        cout << "t4 value: " << value << std::endl;
        std::thread t5 = std::move(std::thread(hello , 5));
        if (t5.joinable()) {
            runthread(std::move(t5));
        }
        if (t5.get_id() == t4.get_id()) {
            cout << "t5 == t4" << '\n';
        }
    }
}

namespace t2
{
    //Iter迭代器类型，T返回值类型
    template <typename Iter , typename T>
    struct accumulate_block
    {
        void operator()(Iter first , Iter last , T& result)
        {
            result = std::accumulate(first , last , result);
        }
    };

    template <typename Iter , typename T>
    T parallel_accumulate(Iter first , Iter last , T init)
    {
        auto length = last - first;
        if (!length) {
            return init;
        }
        const int min_work_per_thread = 100;
        const int max_threads = (length + min_work_per_thread - 1) / min_work_per_thread;
        const int hardware_threads = static_cast<int>(std::thread::hardware_concurrency());
        const int num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2 , max_threads);
        const int block_size = length / num_threads;
        std::vector<T> result(num_threads);
        std::vector<std::thread> threads(num_threads - 1);
        Iter block_start = first;
        for (int i = 0 ; i < num_threads - 1 ; ++i) {
            Iter block_end = block_start;
            std::advance(block_end , block_size);
            threads[i] = std::thread(accumulate_block<Iter,T>() , block_start , block_end , std::ref(result[i]));
            block_start = block_end;
        }
        accumulate_block<Iter,T>()(block_start , last , result[num_threads - 1]);
        std::for_each(threads.begin() , threads.end() , std::mem_fn(&std::thread::join));
        return std::accumulate(result.begin() , result.end() , init);
    }

    void run()
    {
        cout << "hardware threads: " << std::thread::hardware_concurrency() << std::endl;
        constexpr long long max_num = 1000000;
        std::vector<long long> nums(max_num);
        for (int i = 0 ; i < max_num ; ++i) {
            nums[i] = i;
        }
        auto start = std::chrono::high_resolution_clock::now();
        auto t = parallel_accumulate(nums.begin() , nums.end() , 0LL);
        auto end = std::chrono::high_resolution_clock::now();
        cout << t << " in parallel " << (end - start).count() << " ms" << std::endl;
        start = std::chrono::high_resolution_clock::now();
        t = std::accumulate(nums.begin() , nums.end() , 0LL);
        end = std::chrono::high_resolution_clock::now();
        cout << t << " in common " << (end - start).count() << " ms" << std::endl;
    }
}

namespace t3
{
    constexpr int MAX = 10000;
    std::vector<int> nums(MAX);
    std::mutex nums_mutex;

    void add(int index , int value)
    {
        for (int i = 0 ; i < 100 ; ++i) {
            nums[index] += value;
        }
        for (int i = 0 ; i < 99 ; ++i) {
            nums[index] -= value;
        }
    }

    void add_mutex(int index , int value)
    {
        nums_mutex.lock();
        add(index , value);
        nums_mutex.unlock();
    }

    void add_lock_guard(int index , int value)
    {
        std::lock_guard<std::mutex> lock(nums_mutex);
        add(index , value);
    }

    using add_fun = void (*)(int index , int value);

    void run_add(add_fun f , int index , int value)
    {
        for (int i = 0 ; i < MAX ; ++i) {
            std::thread t(f , index , value);
            t.detach();
        }
        sleep(1);
        cout << nums[index] << '\n';
    }


    void run()
    {
        run_add(add , 0 , 1);
        run_add(add_mutex , 1 , 1);
        run_add(add_lock_guard , 2 , 1);
    }
}

namespace t4
{
    class safe_stack
    {
        std::vector<int> nums{};
        std::mutex nums_mutex;
        int id{};

    public:
        [[nodiscard]] int getid()
        {
            std::lock_guard<std::mutex> lock(nums_mutex);
            return id;
        }

        safe_stack(int i) : id{i} {};

        void push(int value)
        {
            std::lock_guard<std::mutex> lock(nums_mutex);
            nums.push_back(value);
            cout << "push " << value << '\n';
        }

        bool empty()
        {
            std::lock_guard<std::mutex> lock(nums_mutex);
            return nums.empty();
        }

        void pop(int& value)
        {
            std::lock_guard<std::mutex> lock(nums_mutex);
            if (nums.empty()) {
                cout << "empty!" << '\n';
                return;
            }
            value = nums[nums.size() - 1];
            nums.pop_back();
            cout << "pop " << value << '\n';
        }

        std::shared_ptr<int> pop()
        {
            std::lock_guard<std::mutex> lock(nums_mutex);
            auto t = std::make_shared<int>(nums[nums.size() - 1]);
            nums.pop_back();
            return t;
        }

        bool try_pop(int& value)
        {
            std::lock_guard<std::mutex> lock(nums_mutex);
            if (nums.empty()) {
                return false;
            }
            value = nums[nums.size() - 1];
            nums.pop_back();
            return true;
        }

        friend void swap(safe_stack& stack1 , safe_stack& stack2) noexcept
        {
            if (stack1.nums == stack2.nums) {
                return;
            }
            std::lock(stack1.nums_mutex , stack2.nums_mutex);
            std::lock_guard<std::mutex> lock1(stack1.nums_mutex , std::adopt_lock);
            std::lock_guard<std::mutex> lock2(stack2.nums_mutex , std::adopt_lock);
            cout << "swap" << '\n';
            std::swap(stack1.id , stack2.id);
        }
    };

    void swap(safe_stack& stack1 , safe_stack& stack2) noexcept;

    void run()
    {
        safe_stack stack{1};
        auto pop = static_cast<void(safe_stack::*)(int&)>(&safe_stack::pop);
        for (int i = 0 ; i < 100 ; ++i) {
            if (stack.empty()) {
                std::thread t(&safe_stack::push , &stack , i);
                t.detach();
            }
            else {
                int value{};
                std::thread t(pop , &stack , std::ref(value));
                t.detach();
            }
        }
        safe_stack stack2{2};
        for (int i = 0 ; i < 100 ; ++i) {
            int value{};
            if (stack2.try_pop(value)) {
                cout << "try_pop " << value << '\n';
            }
            else {
                std::thread t(&safe_stack::push , &stack2 , i);
                t.detach();
            }
        }
        int t{};
        std::thread t1(swap , std::ref(stack) , std::ref(stack2));
        t1.join();
        cout << stack.getid() << ' ' << stack2.getid() << '\n';
    }
}

namespace t5
{
    int value{};
    std::mutex mutex;
    int* p{};

    std::unique_lock<std::mutex> getlock()
    {
        std::unique_lock<std::mutex> lock(mutex);
        ++value;
        return lock;
    }

    void runadd(int i)
    {
        std::unique_lock<std::mutex> lock(getlock());
        int t = ++value;
        lock.unlock();
        printf("%d:%d\n" , i , t);
    }

    void errorinit()
    {
        if (!p) {
            std::lock_guard<std::mutex> lock(mutex);
            if (!p) {
                p = new int(5);
            }
        }
        cout << *p << '\n';
    }

    void safeinit(int i)
    {
        std::once_flag flag;
        std::call_once(flag , []() { p = new int(5); });
        printf("%d:%d\n" , i , *p);
    }

    void staticinit(int i)
    {
        static int sp{5};
        printf("%d:%d\n" , i , sp);
    }

    void run()
    {
        for (int i = 0 ; i < 10 ; ++i) {
            std::thread t(runadd , i);
            t.detach();
        }
        sleep(1);
        cout << '\n';
        for (int i = 0 ; i < 10 ; ++i) {
            std::thread t(safeinit , i);
            t.detach();
        }
        sleep(1);
        cout << '\n';
        for (int i = 0 ; i < 10 ; ++i) {
            std::thread t(staticinit , i);
            t.detach();
        }
        sleep(1);
    }
}

namespace t6
{
    bool start{};
    std::mutex mutex;
    std::queue<int> datas{};
    std::condition_variable cv{};


    void sleepforwhile()
    {
        std::unique_lock<std::mutex> lock(mutex);
        while (!start) {
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            cout << "Sleep 100ms...\n";
            lock.lock();
        }
        cout << "Sleep down\n";
    }

    void pushdata()
    {
        int i{};
        while (start) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::lock_guard<std::mutex> lock(mutex);
            datas.push(i++);
            cv.notify_one();
        }
        cout << "end pushdata\n";
    }

    void coutdata()
    {
        int i{};
        while (start) {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock , []() { return !datas.empty(); });
            int t = datas.front();
            datas.pop();
            lock.unlock();
            printf("%d:%d\n" , i++ , t);;
        }
        cout << "end coutdata\n";
    }

    void run()
    {
        std::unique_lock<std::mutex> lock(mutex , std::defer_lock);
        std::thread t1(sleepforwhile);
        t1.detach();
        sleep(1);
        start = true;
        std::thread idata(pushdata);
        std::thread odata(coutdata);
        idata.detach();
        odata.detach();
        sleep(1);
        lock.lock();
        start = false;
        lock.unlock();
        sleep(1);
    }
}

namespace t7
{
    constexpr int num{77};

    class Num
    {
        int i{77};

    public:
        [[nodiscard]] int geti() const
        {
            return i;
        }
    };

    int geti() { return num; }

    int add(int a , int b)
    {
        cout << "run add ";
        return a + b;
    }

    std::future<int> addpromisefuture()
    {
        auto p = std::make_shared<std::promise<int>>();
        auto f = p->get_future();
        std::thread t([p]
        {
            p->set_value(5);
        });
        t.detach();
        return f;
    }

    void waitfutures()
    {
        auto p = std::make_shared<std::promise<int>>();
        std::shared_future<int> f = p->get_future();
        std::thread t([p]
        {
            sleep(1);
            p->set_value(6);
        });
        t.detach();
        for (int i = 0 ; i < 10 ; ++i) {
            std::thread tem([f,i]
            {
                printf("%d:%d\n" , i , f.get());
            });
            tem.detach();
        }
        sleep(1);
    }

    void run()
    {
        auto f1 = std::async(geti);
        auto f2 = std::async(&Num::geti , Num());
        cout << "f1: " << f1.get() << "\nf2: " << f2.get() << '\n';
        auto f3 = std::async(std::launch::deferred , add , 1 , 2);
        cout << "f3: " << f3.get() << '\n';
        std::packaged_task<int(int , int)> addtask(add);
        auto f4 = addtask.get_future();
        addtask(1 , 3);
        cout << f4.get() << '\n';
        cout << addpromisefuture().get() << '\n';
        waitfutures();
    }
}

namespace t8
{
    void run() {}
}

int main()
{
    t7::run();
}
