//
// Created by 33550 on 2026/9/9.
//
#include <iostream>
#include <type_traits>//std::is_constant_evaluated()

//constexpr关键字用于向编译器和其他开发人员表明函数可以在常量表达式中使用
constexpr int add(int a, int b) { return a + b; }
/*Constexpr函数可以在编译时求值，原理是编译器直接运行函数，所以内部可以有非const变量
 *对constexpr函数的调用必须具有在编译时已知（例如是常量表达式）的参数。
 *constexpr函数中的所有语句和表达式都必须在编译时可求值。
 *Constexpr函数也可以在运行时求值，在这种情况下它们将返回一个非constexpr结果。
 *constexpr函数是隐式inline的，这使它们免受一次定义规则的约束
 *编译器必须能够看到constexpr（或consteval）函数的完整定义，而不仅仅是前向声明。
 */

//consteval声明表示必须在编译期调用求值，仅用于声明函数
consteval int sub(int a, int b) { return a - b; }

//constinit声明必须在编译期初始化，但不保证变量是常量不可修改，仅用于声明变量
constinit int gx{add(10, 20)};

//采用如下方式来让constexpr函数强制编译时求值
consteval auto CONSTEVAL(auto value) { return value; }

constexpr int compare(int x, int y) // function is constexpr
{
    if (std::is_constant_evaluated())
        return (x > y ? x : y);
    else
        return (x < y ? x : y);
}


class pair
{
    int m_x{};

public:
    //声明constexpr构造函数以便能声明constexpr对象
    constexpr pair(int x) : m_x{x} {}
    //声明constexpr成员函数以便能在常量表达式中评估
    [[nodiscard]] constexpr int get() const { return m_x; }
};

int main()
{
    //const 意味着对象的值在初始化后不能更改。初始化器值可以在编译时或运行时已知。const 对象可以在运行时求值。
    //使用常量表达式，编译器可以选择在编译时求值来优化性能
    const int x{10};
    const int y{20};
    const int z{x + y}; //z可能会在编译时求值，取决于编译器
    std::cout << "z=" << z << '\n';

    //编译器确定常量表达式有时可能非常困难，我们可以显式告诉编译器进行编译时求值
    //constexpr 意味着对象可以在常量表达式中使用。初始化器值必须在编译时已知，初始化在编译期完成。
    constexpr int w{x + y};
    //constexpr 适用于非整型变量
    constexpr double d1{1.2};
    std::cout << "w=" << w << " d1=" << d1 << '\n';

    /*任何其初始化器为常量表达式的常量变量都应声明为 constexpr。
     *任何其初始化器不是常量表达式的常量变量（使其成为运行时常量）都应声明为 const。
     *constexpr 为对象提供隐式 const
     */

    //C++17 引入了 constexpr if 语句，它要求条件必须是常量表达式。constexpr-if-语句的条件将在编译时评估。
    constexpr double gravity{9.8};
    if constexpr (gravity == 9.8) // now using constexpr if
        std::cout << "Gravity is normal.\n";
    else
        std::cout << "We are not on Earth.\n";
    //当条件是常量表达式时，优先使用 constexpr if 语句而不是非 constexpr if 语句。

    //constexpr 函数是允许在常量表达式中调用的函数
    constexpr int d2{add(x, y)};
    //把constexpr函数包进consteval调用，强制它在编译期求值
    constexpr int d5{CONSTEVAL(add(x, y))};
    std::cout << "d2=" << d2 << " d5=" << d5 << '\n';

    //consteval 函数只能以常量表达式调用
    constexpr int d4{sub(y, x)};
    std::cout << "d4=" << d4 << '\n';

    //constinit 变量在编译期完成初始化，运行时仍然可以修改
    gx += 1;
    std::cout << "gx=" << gx << '\n';

    //同一个constexpr函数在编译期与运行期可以走不同路径
    constexpr int c1{compare(2, 9)}; //编译期路径：返回较大值
    int a{2}, b{9};
    //sub(a, b);   //编译错误：a,b是运行时值，consteval调用必须编译期求值
    std::cout << "c1=" << c1 << " compare=" << compare(a, b) << '\n'; //运行时路径：返回较小值

    //类的constexpr
    constexpr pair p{1};
    constexpr int d3{p.get()};
    std::cout << "d3=" << d3 << '\n';
}
