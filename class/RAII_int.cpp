//
// Created by 33550 on 2026/9/3.
//
#include <iostream>
using std::cout;
using std::endl;

class RAII_int_t
{
private:
    int *m_int_p{};

public:
    RAII_int_t()
    {
        cout << "RAII_int_t(0) default constructed" << endl;
        m_int_p = new int{};
    }

    explicit RAII_int_t(const int &value)
    {
        cout << "RAII_int_t(" << value << ") value constructed" << endl;
        m_int_p = new int{value};
    }

    explicit RAII_int_t(const int *value_p)
    {
        if (value_p != nullptr) {
            cout << "RAII_int_t(" << *value_p << ") value_p constructed" << endl;
            m_int_p = new int{*value_p};
        }
        else {
            cout << "value_p invalid,RAII_int_t(0) constructed" << endl;
            m_int_p = new int{};
        }
    }

    RAII_int_t(const RAII_int_t &other)
    {
        cout << "RAII_int_t(" << other.get() << ") copy constructed" << endl;
        m_int_p = new int{other.get()};
    }

    RAII_int_t &operator=(RAII_int_t other)
    {
        std::swap(m_int_p, other.m_int_p);
        return *this;
    }

    RAII_int_t &operator=(const int &value)
    {
        cout << "RAII_int_t(" << *m_int_p << ") turn to " << value << endl;
        int *int_p = new int{value};
        delete m_int_p;
        m_int_p = int_p;
        return *this;
    }

    operator int() const { return *m_int_p; }

    [[nodiscard]] int get() const { return *m_int_p; }

    ~RAII_int_t()
    {
        cout << "RAII_int_t(" << *m_int_p << ") destroyed" << endl;
        delete m_int_p;
    }
};

int add(const int t1, const int t2) { return t1 + t2; }

int getSum(const RAII_int_t &t1, const RAII_int_t &t2)
{
    RAII_int_t sum{t1 + t2};
    return sum;
}

int main()
{
    RAII_int_t t1;
    cout << t1.get() << endl;
    t1 = 1;
    int t = 2;
    {
        RAII_int_t t2{t};
        cout << t2.get() << endl;
    }
    t = 3;
    int *p = &t;
    RAII_int_t t3{p};
    cout << t3.get() << endl;
    RAII_int_t t4{t3};
    cout << t4.get() << endl;
    t4 = 4;
    cout << add(t1, t3) << endl;
    RAII_int_t t5{getSum(t1, t4)};
    cout << t5.get() << endl;
}
