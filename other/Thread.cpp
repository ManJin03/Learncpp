//
// Created by 33550 on 2026/10/8.
//
#include <algorithm>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>
#include <chrono>
using std::cout;

namespace
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

namespace test
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

int main()
{
    test::run();
}
