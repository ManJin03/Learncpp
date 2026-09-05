//
// Created by 33550 on 2026/9/3.
//
#include <iostream>
#include <vector>
#include <ranges>//c++20
using std::cout;
using std::endl;
using std::vector;

//辅助打印模板函数以支持不同元素类型vector
//T!=bool，vector<bool>被认为不属于vector容器，应该尽量避免使用
template<typename T>
void print(const vector<T> &vec)
{
    cout << vec.size() << ':';
    for (const T &i: vec) { cout << i << " "; }
    cout << endl;
}

//c++20提供了反向for循环遍历方式
template<typename T>
void print_reverse(const vector<T> &vec)
{
    cout << vec.size() << ':';
    for (const T &i: std::views::reverse(vec)) { cout << i << " "; }
    cout << endl;
}

// 或者更加通用的模板，不局限于vector
// template<typename T>
// void print(const T &arr){...}
// 也可auto实现
// void print(const auto &arr){...}


//vector 初始化方式
vector<int> initialize()
{
    cout << "vector initialize begin" << endl;
    //default constructor
    vector<int> empty{};
    print(empty);
    //list constructor
    vector<double> list{1, 2, 3, 4};
    print(list);
    //CTAD (C++17) to deduce element
    vector ctad{'a', 'b', 'c', 'd', 'e'};
    print(ctad);
    //编译错误，10不匹配列表构造，也不匹配单参数构造函数，本身是显示构造不能匹配复制初始化
    //vector<int> v=10;
    //直接初始化显式构造v1，元素默认初始化为0
    vector<int> v1(6);
    print(v1);
    //列表初始化优先于直接初始化，列表构造
    vector<int> v2{6};
    print(v2);
    //复制列表初始化，等价于v3={6}
    vector<int> v3({6});
    print(v3);
    struct foo
    {
        //成员默认初始化器不支持直接初始化
        //vector<int> v4(7);
        //使用复制列表初始化
        vector<int> v5 = {vector<int>(7)};
    };
    foo f;
    print(f.v5);
    //无法将数据成员设置为const
    //vector<const int> v6(8);
    //只能将vector本身设置为const
    const vector v6{9, 8, 7};
    print(v6);
    //也无法设置为constexpr(原因是无法将成员设置为const）
    //constexpr vector v6{3};
    cout << "vector initialize end" << endl;
    return {0, 1, 2, 3};
}

//vector 访问方式
void access(const vector<int> &vec)
{
    cout << "access vector begin" << endl;
    //使用[]访问元素（无边界检查）
    cout << vec[2] << endl;
    //适用at()访问元素（越界抛出异常）
    cout << vec.at(3) << ' ';
    try { cout << vec.at(4) << endl; } catch (std::out_of_range &e) { cout << e.what() << endl; }
    //&来取地址（连续地址）
    for (const int &i: vec) { cout << &i << endl; }
    //size()来获取数组大小，返回type_size
    cout << vec.size() << endl;
    //或者使用c++17引入的std::size()，该方法可接受c风格数组，适用范围广
    cout << std::size(vec) << endl;
    //c++20引入ssize()，返回有符号整型，通常是std::ptrdiff_t
    cout << std::ssize(vec) << endl;
    //最好适用std::size_t作为索引类型，其他类型（非constexpr）会发生类型窄化警告（constexpr不会)
    constexpr std::size_t/*vector<int>::size_type*/ index = 1;
    cout << vec.at(index) << endl;
    //或者使用data()获取原始c风格数组进行索引,p=const int *
    auto p = vec.data();
    cout << p[index] << endl;
    //front()和back()返回首尾元素
    cout << vec.front() << ' ' << vec.back() << endl;
    cout << "vector access end" << endl;
}

void modifier(vector<int> &vec)
{
    cout << "vector modifier begin" << endl;
    //使用resize(n)调整vector大小（运行时）
    vec.resize(1000);
    cout << vec.size() << endl;
    vec.resize(5);
    cout << vec.size() << endl;
    //使用capacity获取vector容量，即已分配的内存，不会小于vector大小
    cout << vec.capacity() << endl;
    //使用shrink_to_fit()去除没有使用的容量
    vec.shrink_to_fit();
    cout << vec.capacity() << endl;
    //使用reserve(n)重新分配容量
    vec.reserve(10);
    cout << vec.capacity() << endl;
    //使用push_back(n)来模拟入栈
    vec.push_back(4);
    print(vec);
    //使用emplace_back(n)来模拟入栈，只适合想添加临时对象或使用显示构造函数
    vec.emplace_back(5);
    print(vec);
    //使用pop_back()来模拟出栈
    vec.pop_back();
    print(vec);
    //使用clear()清空vector
    vec.clear();
    print(vec);
    //使用assign(n,num)来重新赋值vector（可用{列表}或(begin到end)的迭代器）
    vec.assign(6, {1});
    print(vec);
    //使用insert(it,...)插入元素(value)，列表({})，n个元素(n,value),范围(it1,it2)，返回迭代器
    auto it = vec.insert(vec.begin(), 2);
    print(vec);
    //使用erase清除it迭代器元素
    vec.erase(it);
    print(vec);
    //使用swap(other)交换两个vector
    vector vec2{3,2,1};
    vec.swap(vec2);
    print(vec);
    print(vec2);
    cout << "vector modifier end" << endl;
}

int main()
{
    //vector 支持移动构造
    vector<int> vec = initialize();
    cout << "get vec as:" << endl;
    print(vec);
    cout << "and reverse like" << endl;
    print_reverse(vec);
    access(vec);
    modifier(vec);
}
