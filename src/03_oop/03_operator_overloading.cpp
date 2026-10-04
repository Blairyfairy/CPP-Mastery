// Topics: operator overloading (member, global, friend), assignment operator, smart-pointer
//         basics (operator* and ->), type conversions, member initialization list
#include "check.hpp"
#include <iostream>
#include <string>

class Vec2 {
    double x_, y_;

public:
    // Member initialization list: initializes (rather than assigns) members
    Vec2(double x = 0, double y = 0) : x_{x}, y_{y} {}

    Vec2 operator+(const Vec2& o) const { return {x_ + o.x_, y_ + o.y_}; }
    Vec2& operator+=(const Vec2& o) { x_ += o.x_; y_ += o.y_; return *this; }
    Vec2& operator=(const Vec2&) = default;  // assignment operator
    bool operator==(const Vec2& o) const { return x_ == o.x_ && y_ == o.y_; }
    bool operator!=(const Vec2& o) const { return !(*this == o); }
    double& operator[](int i) { return i == 0 ? x_ : y_; }
    Vec2 operator-() const { return {-x_, -y_}; }
    Vec2& operator++() { ++x_; ++y_; return *this; }  // prefix
    Vec2 operator++(int) { Vec2 old{*this}; ++*this; return old; }  // postfix

    // friend: non-member with access to private state
    friend std::ostream& operator<<(std::ostream& os, const Vec2& v) {
        return os << '(' << v.x_ << ", " << v.y_ << ')';
    }
    friend Vec2 operator*(double k, const Vec2& v);  // global overload, scalar on the left
};
Vec2 operator*(double k, const Vec2& v) { return {k * v.x_, k * v.y_}; }

// Smart-pointer basics: overload * and -> so an object behaves like a pointer.
template <typename T>
class SimplePtr {
    T* p_;

public:
    explicit SimplePtr(T* p) : p_{p} {}
    ~SimplePtr() { delete p_; }
    SimplePtr(const SimplePtr&) = delete;
    SimplePtr& operator=(const SimplePtr&) = delete;
    T& operator*() const { return *p_; }
    T* operator->() const { return p_; }
};

// Type conversions
class Meters {
    double m_;

public:
    explicit Meters(double m) : m_{m} {}           // explicit: no silent double -> Meters
    explicit operator double() const { return m_; }  // user type -> primitive
};
class Feet {
    double f_;

public:
    Feet(double f) : f_{f} {}                       // implicit primitive -> user type
    Feet(const Meters& m) : f_{static_cast<double>(m) * 3.28084} {}  // user -> user
    double value() const { return f_; }
};

int main() {
    SECTION("arithmetic & comparison operators");
    Vec2 a{1, 2}, b{3, 4};
    Vec2 c = a + b;
    CHECK(c == Vec2(4, 6));
    c += a;
    CHECK(c[0] == 5 && c[1] == 8);
    CHECK(-a == Vec2(-1, -2));
    CHECK(a != b);
    Vec2 d{1, 1};
    CHECK(d++ == Vec2(1, 1) && d == Vec2(2, 2));
    CHECK((++d) == Vec2(3, 3));

    SECTION("global/friend operators");
    CHECK(2 * a == Vec2(2, 4));
    std::cout << "a=" << a << " 2*b=" << 2 * b << "\n";

    SECTION("smart pointer basics");
    SimplePtr<std::string> sp{new std::string("hello")};
    CHECK(sp->size() == 5 && (*sp)[0] == 'h');

    SECTION("type conversions");
    Meters m{10};
    double raw = static_cast<double>(m);
    Feet f = m;       // Meters -> Feet through converting ctor
    Feet g = 3.0;     // double -> Feet through converting ctor
    CHECK(raw == 10 && f.value() > 32.8 && f.value() < 32.9 && g.value() == 3.0);
    std::cout << "OK\n";
}
