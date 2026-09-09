//
// Created by 33550 on 2026/9/5.
//
#include <iostream>
#include<string>
#include <sstream>
using std::cout;
using std::endl;
using std::string;

string initialize()
{
    cout << "initialize" << endl;
    string s1;
    cout << s1 << endl;

    string s2{"my string"};
    //s2="my string";
    //s2.assign("my string");
    cout << s2 << endl;

    string s3{s2};
    //s3=s2;
    cout << s3 << endl;
    s1 = s2 = s3;

    string s4{s3, 3};
    cout << s4 << endl;

    string s5{s3, 3, 3};
    cout << s5 << endl;

    //必须使用直接初始化，列表初始化会以字符形式解析数字
    string s6(4, '6');
    cout << s6 << endl;

    string s7{s2.begin(), s2.end()};
    cout << s7 << endl;

    //to_string()在某些情况下会和cout的显示不一样
    string s8{std::to_string(666)};
    cout << s8 << endl;
    //更推荐下列写法：
    std::ostringstream ss;
    ss << 7777;
    string s9{ss.str()};
    cout << s9 << endl;

    string s10;
    s10.assign(s1, 3, 3);
    cout << s10 << endl;
    cout << "initialize done" << endl;
    return s2;
}

void access(std::string &s)
{
    cout << "access" << endl;
    //均返回string大小
    cout << s.length() << endl;
    cout << s.size() << endl;

    //返回是否为空
    cout << static_cast<int>(s.empty()) << endl;

    //返回结尾是否为该字符串
    cout << s.ends_with("ing") << endl;

    cout << s.compare("my string") << endl;
    cout << (s == "my string") << endl;
    cout << (s < "my") << endl;

    //字符串允许的最大字符数
    cout << s.max_size() << endl;

    //字符串容量
    cout << s.capacity() << endl;

    //at()有安全检查
    cout << s[3] << endl;
    cout << s.at(3) << endl;

    cout << s.c_str() << endl; //const
    cout << s.data() << endl;
    //copy()更加危险，避免使用
    char p[3];
    s.copy(p, 2, 0);
    p[2] = '\0';
    cout << p << endl;

    cout << s.front() << ' ' << s.back() << endl;

    auto it = s.find("str");
    cout << it << endl;
    cout << "access done" << endl;
}

void modified(string &s)
{
    cout << "modified" << endl;
    //可能和请求的容量大小不一样
    s.reserve(100);
    cout << s.capacity() << endl;
    /*s.reserve()被弃用*/
    s.shrink_to_fit();
    cout << s.capacity() << endl;

    string s1{"your string"};
    s.swap(s1);
    cout << s << endl;
    cout << s1 << endl;

    s += "!";
    //append()用于拼接字符串
    s1.append("!");
    s1.append(s, 0, 4);
    cout << s << endl;
    cout << s1 << endl;

    s.push_back('6');
    s1.append(6, '6');
    cout << s << endl;
    cout << s1 << endl;

    s1.insert(3, s);
    s.insert(5, "own ");
    cout << s << endl;
    cout << s1 << endl;

    s1.pop_back();
    s.insert(4, s1, 7, 6);
    s1.insert(5, 7, '7');
    cout << s << endl;
    cout << s1 << endl;

    auto it = s.find("str");
    s.erase(it);
    cout << s << endl;
    it = s1.rfind("str");
    cout << s1 << endl;

    int pos{3}, count{3};
    s1.replace(pos, count, s);
    cout << s1 << endl;

    char t[10]{};
    //只能将自身copy到c风格字符数组中
    s.copy(t, count, pos);
    cout << t << endl;
    cout << "modified done" << endl;
}

void view(string &s)
{
    cout << "string_view" << endl;
    //string_view创建一个string的视图，拥有和string相同的只读函数，
    //但是无法修改，生命周期绑定至string，可以看作是类似指针的用法。
    std::string_view sv{s};

    using namespace std::string_literals; // access the s suffix
    using namespace std::string_view_literals; // access the sv suffix

    std::cout << "foo\n"; // no suffix is a C-style string literal
    std::cout << "goo\n"s; // s suffix is a std::string literal
    std::cout << "moo\n"sv; // sv suffix is a std::string_view literal

    std::string_view s1{"my string"}; //安全，s1指向常量字符串
    std::string_view s2{"my string"s}; //危险，s2指向一个被转换成string的临时对象，s2在该语句结束后悬空
    //收缩视图，不可逆操作
    sv.remove_prefix(1);
    sv.remove_suffix(1);
    cout << sv << endl;
}

int main()
{
    //移动构造
    string s{initialize()};
    access(s);
    modified(s);
    view(s);
}
