//
// Created by 33550 on 2026/9/1.
//
#include<iostream>

class test1 {
private:
    int m_x;
    int m_y{};

public:
    test1() {}

    void print() const {
        std::cout << m_x << "/" << m_y << std::endl;
    }
};

class test2 {
private:
    int m_x;
    int m_y{};

public:
    //不带默认构造函数，编辑器自动生成

    //使用显示默认构造函数的效果一样
    //test2() = default;

    void print() const {
        std::cout << m_x << "/" << m_y << std::endl;
    }
};

int main() {
    std::cout << "默认初始化的用户默认构造" << std::endl;
    test1 t1;
    t1.print();
    std::cout << "列表初始化的用户默认构造" << std::endl;
    test1 t2{};
    t2.print();
    std::cout << "默认初始化的编译器默认构造" << std::endl;
    test2 t3;
    t3.print();
    std::cout << "列表初始化的编译器默认构造" << std::endl;
    test2 t4{};
    t4.print();
    std::cout << "直接初始化一个右值对象赋值初始化" << std::endl;
    test2 t5 = test2(); //test2{}也可以
    t5.print();
}
