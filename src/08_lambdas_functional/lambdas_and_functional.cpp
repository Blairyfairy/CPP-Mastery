// Topics: callbacks (function pointers, function objects), lambda expressions & captures,
//         generalized capture (C++14), std::function, std::bind, constexpr lambda (C++17)
#include "check.hpp"
#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

// 1) callback via function pointer
int Twice(int x) { return 2 * x; }
int ApplyFp(int (*f)(int), int v) { return f(v); }

// 2) callback via function object (stateful)
struct Multiplier {
    int k;
    int operator()(int x) const { return k * x; }
};

// 3) std::function accepts any callable with a matching signature
int ApplyFn(const std::function<int(int)>& f, int v) { return f(v); }

int Volume(int l, int w, int h) { return l * w * h; }

class Counter {
public:
    int base = 100;
    auto MakeAdder() { return [this](int x) { return base + x; }; }  // captures this
};

int main() {
    SECTION("callbacks");
    CHECK(ApplyFp(Twice, 4) == 8);
    CHECK(Multiplier{3}(5) == 15);

    SECTION("lambda basics");
    auto square = [](int x) { return x * x; };
    CHECK(square(6) == 36);
    std::vector<int> v{5, 2, 8, 1};
    std::sort(v.begin(), v.end(), [](int a, int b) { return a > b; });
    CHECK(v.front() == 8);
    auto isEven = [](int x) -> bool { return x % 2 == 0; };  // explicit return type
    CHECK(std::count_if(v.begin(), v.end(), isEven) == 2);

    SECTION("capture lists");
    int x = 10;
    auto byValue = [x]() { return x; };          // snapshot at creation
    auto byRef = [&x]() { return x; };           // live view
    auto mutableCopy = [x]() mutable { return ++x; };  // modifies the lambda's own copy
    x = 20;
    CHECK(byValue() == 10 && byRef() == 20);
    CHECK(mutableCopy() == 11 && mutableCopy() == 12 && x == 20);
    int a = 1, b = 2;
    auto all = [=]() { return a + b; };          // capture everything by value
    auto allRef = [&]() { a += 10; };            // capture everything by reference
    allRef();
    CHECK(all() == 3 && a == 11);
    Counter c;
    CHECK(c.MakeAdder()(5) == 105);

    SECTION("generalized (init) capture");
    auto owned = std::make_unique<int>(41);
    auto fn = [p = std::move(owned)]() { return *p + 1; };  // move-only capture
    CHECK(owned == nullptr && fn() == 42);
    auto counter = [n = 0]() mutable { return ++n; };
    CHECK(counter() == 1 && counter() == 2);

    SECTION("constexpr lambda (C++17)");
    constexpr auto cube = [](int n) { return n * n * n; };
    static_assert(cube(3) == 27, "lambdas are implicitly constexpr when possible");
    CHECK(cube(4) == 64);

    SECTION("std::function");
    std::function<int(int)> f = Twice;       // function pointer
    CHECK(ApplyFn(f, 3) == 6);
    f = Multiplier{4};                        // functor
    CHECK(ApplyFn(f, 3) == 12);
    f = [](int n) { return n + 1; };          // lambda
    CHECK(ApplyFn(f, 3) == 4);
    std::function<int(int)> empty;
    CHECK(!empty);

    SECTION("std::bind");
    using namespace std::placeholders;
    auto fixedHeight = std::bind(Volume, _1, _2, 10);  // bind the 3rd argument
    CHECK(fixedHeight(2, 3) == 60);
    auto swapped = std::bind(Volume, _3, _2, _1);       // reorder arguments
    CHECK(swapped(1, 2, 3) == 6);
    Multiplier m{5};
    auto boundObj = std::bind(&Multiplier::operator(), &m, _1);  // bind member function
    CHECK(boundObj(4) == 20);
    std::cout << "OK\n";
}
