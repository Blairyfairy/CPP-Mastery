// Topics: OOP basics, class, constructor & destructor, structures, non-static data member
//         initializers (C++11), this pointer, static members, const member functions,
//         delegating constructors (C++11), default & deleted functions (C++11)
#include "check.hpp"
#include <string>

class Employee {
    // Non-static data member initializers (C++11): defaults for every constructor
    std::string name_{"unknown"};
    int age_{0};
    double salary_{0.0};
    static int count_;  // shared by all objects

public:
    Employee() = default;  // ask the compiler for the default constructor
    Employee(std::string name, int age, double salary)
        : name_{std::move(name)}, age_{age}, salary_{salary} { ++count_; }
    // Delegating constructor (C++11): forwards to another constructor
    Employee(std::string name, int age) : Employee{std::move(name), age, 1000.0} {}
    ~Employee() { if (age_ > 0) --count_; }

    Employee(const Employee&) = delete;  // deleted function: not copyable
    Employee& operator=(const Employee&) = delete;

    // const member functions promise not to modify the object
    const std::string& Name() const { return name_; }
    int Age() const { return age_; }
    double Salary() const { return salary_; }

    // returning *this enables chaining
    Employee& SetAge(int a) { this->age_ = a; return *this; }
    Employee& Raise(double pct) { salary_ *= 1 + pct / 100; return *this; }

    static int Count() { return count_; }
};
int Employee::count_ = 0;

// struct: same as class, but members are public by default
struct Point {
    int x{0};
    int y{0};
};

int main() {
    SECTION("constructors, NSDMI, defaults");
    Employee blank;
    CHECK(blank.Name() == "unknown" && blank.Age() == 0);
    CHECK(Employee::Count() == 0);
    {
        Employee e{"Ada", 36, 5000};
        Employee e2{"Linus", 50};  // delegating constructor
        CHECK(Employee::Count() == 2);
        CHECK(e2.Salary() == 1000.0);

        SECTION("this pointer & method chaining");
        e.SetAge(37).Raise(10);
        CHECK(e.Age() == 37);
        CHECK(e.Salary() > 5499.99 && e.Salary() < 5500.01);
    }  // destructors run here
    CHECK(Employee::Count() == 0);

    SECTION("structs");
    Point p;
    Point q{3, 4};
    CHECK(p.x == 0 && q.y == 4);
    // Employee copy = blank;  // error: copy constructor is deleted
    std::cout << "OK\n";
}
