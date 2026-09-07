//
// Created by 33550 on 2026/9/5.
//
#include <iostream>
#include<vector>
using std::cout;
using std::endl;
using std::vector;

int main()
{
    vector vec{0, 11, 22, 33, 44, 55, 66, 66, 66, 77};
    auto begin{std::begin(vec)};
    cout << *begin << endl;
    auto end{vec.end()};
    cout << *(end - 1) << endl;
    for (auto p{begin}; p != end; ++p) { cout << *p << endl; }
    auto it = std::find(begin, end, 3);
    if (it == end) { cout << "Not found 3" << endl; }
    it = std::find(begin, end, 33);
    if (it != end) { cout << *it << " is in pos" << it - begin << endl; }
    //必须重赋值，否则迭代器会失效，erase()返回下一个元素
    it = vec.erase(it);
    auto f = [](const int &t) { return t == 22; };
    it = std::find_if(begin, end, f);
    if (it != end) { cout << *it << " is in pos" << it - begin << endl; }
    auto count = std::count(begin, end, 66);
    cout << count << " ==66" << endl;
    count = std::count_if(begin, end, [](int t) { return t < 40; });
    cout << count << " <40" << endl;
    std::sort(begin, end, std::greater{});
    std::for_each(std::next(begin), end, [](int t) { cout << t << ' '; });
    
    return 0;
}
