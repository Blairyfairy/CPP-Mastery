// Topics: sequence containers, associative containers, unordered containers & std::hash,
//         algorithms, Big-O notes, C++11 container additions (emplace, cbegin, shrink_to_fit...)
#include "check.hpp"
#include <algorithm>
#include <array>
#include <deque>
#include <forward_list>
#include <iterator>
#include <list>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Person {
    std::string name;
    int age;
    bool operator==(const Person& o) const { return name == o.name && age == o.age; }
};
// custom std::hash specialization so Person can be an unordered_set key
namespace std {
template <>
struct hash<Person> {
    size_t operator()(const Person& p) const {
        return hash<string>{}(p.name) ^ (hash<int>{}(p.age) << 1);
    }
};
}  // namespace std

int main() {
    SECTION("sequence containers");
    std::array<int, 3> arr{3, 1, 2};  // fixed size, stack allocated: O(1) access
    std::sort(arr.begin(), arr.end());
    CHECK(arr[0] == 1 && arr.size() == 3);

    std::vector<int> vec{1, 2, 3};     // contiguous: O(1) index, amortized O(1) push_back
    vec.shrink_to_fit();
    std::deque<int> dq{2, 3};          // O(1) push at both ends
    dq.push_front(1);
    CHECK(dq.front() == 1 && dq.size() == 3);
    std::list<int> lst{1, 2, 4};       // doubly linked: O(1) insert at iterator
    auto it = std::next(lst.begin(), 2);
    lst.insert(it, 3);
    CHECK(lst.size() == 4 && *std::next(lst.begin(), 2) == 3);
    std::forward_list<int> fl{1, 2, 3};  // singly linked
    fl.push_front(0);
    CHECK(fl.front() == 0);

    SECTION("associative containers (ordered, O(log n))");
    std::set<int> s{5, 1, 3, 3};  // unique + sorted
    CHECK(s.size() == 3 && *s.begin() == 1);
    std::multiset<int> ms{1, 1, 2};
    CHECK(ms.count(1) == 2);
    std::map<std::string, int> m;
    m["b"] = 2; m["a"] = 1;
    m.emplace("c", 3);
    CHECK(m.begin()->first == "a" && m.at("c") == 3);
    auto ins = m.insert({"a", 99});  // insert does not overwrite
    CHECK(!ins.second && m["a"] == 1);
    std::multimap<int, std::string> mm{{1, "x"}, {1, "y"}};
    CHECK(mm.count(1) == 2);

    SECTION("unordered containers (hashed, O(1) average) & std::hash");
    std::unordered_map<std::string, int> um{{"one", 1}, {"two", 2}};
    CHECK(um.at("two") == 2 && um.count("three") == 0);
    std::unordered_set<Person> people{{"Ann", 30}, {"Bob", 25}};
    CHECK(people.count(Person{"Ann", 30}) == 1);
    CHECK(std::hash<std::string>{}("x") == std::hash<std::string>{}("x"));

    SECTION("algorithms");
    std::vector<int> v{4, 8, 15, 16, 23, 42};
    CHECK(std::accumulate(v.begin(), v.end(), 0) == 108);
    CHECK(*std::find(v.begin(), v.end(), 16) == 16);
    CHECK(std::find_if(v.begin(), v.end(), [](int x) { return x > 20; }) == v.begin() + 4);
    CHECK(std::binary_search(v.begin(), v.end(), 23));
    std::vector<int> sq(v.size());
    std::transform(v.begin(), v.end(), sq.begin(), [](int x) { return x * x; });
    CHECK(sq[1] == 64);
    auto [mn, mx] = std::minmax_element(v.begin(), v.end());
    CHECK(*mn == 4 && *mx == 42);
    v.erase(std::remove_if(v.begin(), v.end(), [](int x) { return x % 2; }), v.end());  // erase-remove idiom
    CHECK(v.size() == 4);  // 15 and 23 removed
    std::reverse(v.begin(), v.end());
    CHECK(v.front() == 42);
    std::vector<int> iotaV(5);
    std::iota(iotaV.begin(), iotaV.end(), 10);
    CHECK(iotaV.back() == 14);
    CHECK(std::all_of(iotaV.begin(), iotaV.end(), [](int x) { return x >= 10; }));
    std::vector<Person> ppl{{"Zed", 20}, {"Amy", 40}, {"Bob", 30}};
    std::sort(ppl.begin(), ppl.end(), [](const Person& a, const Person& b) { return a.age < b.age; });
    CHECK(ppl.front().name == "Zed" && ppl.back().name == "Amy");
    std::cout << "OK\n";
}
