// Topics: inheritance & composition, access modifiers, inheriting constructors (C++11),
//         virtual functions, override & final (C++11), object slicing, typeid, dynamic_cast,
//         abstract classes, multiple (diamond) inheritance, error handling
#include "check.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>

// ------- Bank account hierarchy (mini project) -------
class InsufficientFunds : public std::runtime_error {
public:
    InsufficientFunds() : std::runtime_error("insufficient funds") {}
};

class Account {
protected:  // visible to derived classes only
    double balance_;

public:
    explicit Account(double b = 0) : balance_{b} {}
    virtual ~Account() = default;  // virtual destructor: delete through base is safe
    virtual void Withdraw(double amt) {
        if (amt > balance_) throw InsufficientFunds{};
        balance_ -= amt;
    }
    void Deposit(double amt) { balance_ += amt; }
    double Balance() const { return balance_; }
    virtual std::string Kind() const { return "Account"; }
};

class Savings : public Account {
    double rate_;

public:
    Savings(double b, double rate) : Account{b}, rate_{rate} {}
    void AddInterest() { balance_ += balance_ * rate_; }
    std::string Kind() const override { return "Savings"; }  // override: compiler-checked
};

class Checking : public Account {
    double overdraft_;

public:
    using Account::Account;  // inheriting constructors (C++11)
    void Withdraw(double amt) override {
        if (amt > balance_ + overdraft_) throw InsufficientFunds{};
        balance_ -= amt;
    }
    std::string Kind() const override { return "Checking"; }
};

class Premium final : public Savings {  // final: cannot be derived from
public:
    using Savings::Savings;
    std::string Kind() const override final { return "Premium"; }
};

// ------- Composition: "has-a" -------
class Customer {
    std::string name_;
    Savings savings_;  // Customer HAS a Savings account

public:
    Customer(std::string n, double b) : name_{std::move(n)}, savings_{b, 0.02} {}
    Savings& savings() { return savings_; }
};

// ------- Abstract class / interface -------
class Shape {
public:
    virtual ~Shape() = default;
    virtual double Area() const = 0;  // pure virtual
};
class Square : public Shape {
    double s_;

public:
    explicit Square(double s) : s_{s} {}
    double Area() const override { return s_ * s_; }
};
class Circle : public Shape {
    double r_;

public:
    explicit Circle(double r) : r_{r} {}
    double Area() const override { return 3.14159 * r_ * r_; }
};

// ------- Diamond inheritance with virtual bases -------
struct Device { int id{1}; };
struct Scanner : virtual Device {};
struct Printer : virtual Device {};
struct Copier : Scanner, Printer {};  // only ONE Device subobject thanks to `virtual`

int main() {
    SECTION("polymorphism via base pointers");
    std::vector<std::unique_ptr<Account>> accts;
    accts.push_back(std::make_unique<Account>(100));
    accts.push_back(std::make_unique<Savings>(200, 0.05));
    accts.push_back(std::make_unique<Checking>(50));
    CHECK(accts[1]->Kind() == "Savings" && accts[2]->Kind() == "Checking");

    SECTION("exceptions & overridden behaviour");
    bool threw = false;
    try { accts[0]->Withdraw(500); } catch (const InsufficientFunds&) { threw = true; }
    CHECK(threw);
    accts[0]->Withdraw(40);
    CHECK(accts[0]->Balance() == 60);

    SECTION("object slicing");
    Savings s{100, 0.1};
    Account sliced = s;  // copies only the Account part; the dynamic type is lost
    CHECK(sliced.Kind() == "Account" && s.Kind() == "Savings");

    SECTION("typeid & dynamic_cast");
    Account* base = accts[1].get();
    CHECK(typeid(*base) == typeid(Savings));
    if (auto* sv = dynamic_cast<Savings*>(base)) { sv->AddInterest(); }
    CHECK(base->Balance() > 209.99 && base->Balance() < 210.01);
    CHECK(dynamic_cast<Savings*>(accts[0].get()) == nullptr);  // failed cast => nullptr

    SECTION("composition, final");
    Customer cust{"Ada", 1000};
    cust.savings().AddInterest();
    CHECK(cust.savings().Balance() > 1019.9);
    Premium prem{500, 0.01};
    CHECK(prem.Kind() == "Premium");

    SECTION("abstract classes");
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Square>(2));
    shapes.push_back(std::make_unique<Circle>(1));
    double total = 0;
    for (const auto& sh : shapes) total += sh->Area();
    CHECK(total > 7.14 && total < 7.15);
    // Shape sh;  // error: cannot instantiate an abstract class

    SECTION("diamond inheritance");
    Copier cp;
    cp.id = 9;  // unambiguous because Device is a virtual base
    CHECK(cp.Scanner::id == 9 && cp.Printer::id == 9);
    std::cout << "OK\n";
}
