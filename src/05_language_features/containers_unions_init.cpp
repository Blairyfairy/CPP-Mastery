// Topics: std::initializer_list (C++11), std::vector, unions (tagged), constexpr if basics
#include "check.hpp"
#include <initializer_list>
#include <string>
#include <vector>

// A function/ctor taking std::initializer_list enables {a, b, c} syntax.
class IntBag {
    std::vector<int> v_;

public:
    IntBag(std::initializer_list<int> init) : v_{init} {}
    int Sum() const { int s = 0; for (int x : v_) s += x; return s; }
    std::size_t size() const { return v_.size(); }
};

// Tagged union: the tag records which member is active (safe usage of a union).
struct Value {
    enum class Type { Int, Float } type;
    union {
        int i;
        float f;
    };
    static Value FromInt(int v) { Value x; x.type = Type::Int; x.i = v; return x; }
    static Value FromFloat(float v) { Value x; x.type = Type::Float; x.f = v; return x; }
    double AsDouble() const { return type == Type::Int ? i : f; }
};

int main() {
    SECTION("std::initializer_list");
    IntBag bag{1, 2, 3, 4};
    CHECK(bag.Sum() == 10 && bag.size() == 4);

    SECTION("std::vector");
    std::vector<int> v;
    v.reserve(10);
    for (int i = 0; i < 5; ++i) v.push_back(i);
    CHECK(v.size() == 5 && v.capacity() >= 10);
    v.insert(v.begin() + 1, 99);
    CHECK(v[1] == 99);
    v.erase(v.begin());
    CHECK(v.front() == 99);
    v.pop_back();
    CHECK(v.size() == 4);
    std::vector<std::string> names{"a", "b"};
    names.emplace_back("c");  // constructs in place
    CHECK(names.back() == "c");
    std::vector<int> grid(3, 7);  // 3 elements, all 7
    CHECK(grid.size() == 3 && grid[2] == 7);

    SECTION("unions");
    Value a = Value::FromInt(3), b = Value::FromFloat(2.5f);
    CHECK(a.AsDouble() == 3.0 && b.AsDouble() == 2.5);
    CHECK(sizeof(Value::i) == sizeof(int));
    std::cout << "OK\n";
}
