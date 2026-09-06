//
// Created by 33550 on 2026/9/6.
//
#include <iostream>
#include<unordered_map>
#include<string>
using std::cout;
using std::endl;
using std::unordered_map;
using std::string;

int main()
{
    unordered_map<string, int> map;
    cout << map.bucket_count() << endl;
    map["apple"] = 1;
    map.insert({"banana", 2});
    std::pair<string, int> pair{"orange", 3};
    map.insert(pair);
    if (!map.empty()) { cout << map.size() << endl; }
    if (map.contains("apple"))
        cout << map.count("apple") << endl; //实际上map不允许重复，count取值只有0或1
    cout << map.load_factor() << endl;
    cout << map.max_load_factor() << endl;
    cout << map.bucket_count() << endl;
    cout << map.max_bucket_count() << endl;
    auto it = map.bucket("banana");
    cout << map.bucket_size(it) << endl;
    //extract(key)是不重新分配内存情况下唯一可修改map中元素键值的方式
    auto node = map.extract("orange");
    node.key() = "mongo";
    map.insert(std::move(node));
    unordered_map<string, int> t{{"phone", 5000}, {"computer", 10000}};
    map.merge(t);
    cout << map.size() << endl;
    if (t.empty()) { cout << "t is empty" << endl; }
}
