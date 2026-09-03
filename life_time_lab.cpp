//
// Created by 33550 on 2026/9/3.
//
#include<iostream>
#include <string>
using std::cout;
using std::endl;

class Test
{
private:
    std::string m_name{};
    unsigned int m_id{};

public:
    Test() : Test{""} {}

    explicit Test(const std::string &name, const unsigned int &id = 0)
        : m_name{name}, m_id{id} { cout << m_name << " has been created(id:" << m_id << ')' << endl; };

    Test(const Test &test)
        : m_name{test.m_name}, m_id{test.m_id} { cout << m_name << " has been copied(id:" << m_id << ')' << endl; }

    ~Test() { cout << m_name << " has been deleted(id:" << m_id << ')' << endl; }
    void setName(const std::string &name) { m_name = name; }
    void setId(const unsigned int &id) { m_id = id; }
    std::string getName() const { return m_name; }
    unsigned int getId() const { return m_id; }

    friend void print(const Test &test);
};

void print(const Test &test) { cout << test.m_name << ' ' << test.m_id << endl; }

static Test add(const Test &test1, const Test &test2)
{
    std::string name{test1.getName() + test2.getName()};
    unsigned int id{test1.getId() + test2.getId()};
    Test test{name, id};
    test.setId(4);
    Test testtest{test};
    testtest.setId(5);
    return test;
}

int main()
{
    Test *test1 = new Test{"second"};
    print(*test1);
    test1->setId(2);
    Test test2{*test1};
    print(test2);
    test1->setName("first");
    test1->setId(1);
    print(*test1);
    Test test3{"third", 3};
    print(test3);
    Test test4{add(*test1, test2)};
    delete test1;
    print(test4);
}
