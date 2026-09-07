# LearnCpp

## 简介

A practice product to learn enhanced cpp.

该项目旨在记录本人在学习c++时写的体现c++各种性质的测试代码，如有不妥或遗漏之处，欢迎指正。

## 文件目录

| 文件名                                               | 内容                                                | 
|------------------------------------------------------|-----------------------------------------------------|
| [default_initialized](class/default_initialized.cpp) | 不同形式的默认构造函数对参数的初始化方式不同        |
| [life_time_lab](class/life_time_lab.cpp)             | 对象的创建方式和释放的时机，即生命周期              |
| [RAII_int](class/RAII_int.cpp)                       | 一个自我管理内存创建和释放的int智能类               |               
| [move_function](class/move_function.cpp)             | 实现了移动构造和移动赋值的类，附带std::move的测试   |
| [unique_ptr]()                                       | 实现类似std::unique的智能指针，固定类型不使用模板类 |
| [RAII_file]()                                        | 尝试实现管理文件生命的类和其基础功能                |
| [string_func](stl/string_func.cpp)                   | string方法示例，附带string_view                     |
| [iterator_use](stl/iterator_use.cpp)                 | 迭代器的使用                                        |
| [vector_func](stl/vector_func.cpp)                   | vector方法示例                                      |
| [unordered_map_func](stl/unordered_map_func.cpp)     | unordered_map方法示例                               |
| [constexpr_lab]()                                    | 常量表达式实验                                      |
| [process_lab]()                                      | 底层接口进程api实验                                 |
| [c++man]()                                           | c++实现的猜词游戏                                   |
|                                                      |                                                     |

## 说明

文件内附有详细或简略的注释，多有疏漏之处，可前往[learncpp](https://learncpp.cn/)学习 ，
更加详细的资料请前往[cppreference](https://cppreference.cn/)