// Topics: enums & scoped enums (C++11), raw strings, std::string, string streams,
//         user-defined literals, constexpr
#include "check.hpp"
#include <sstream>
#include <string>

enum Color { Red, Green = 5, Blue };          // unscoped: leaks names, converts to int
enum class Level : unsigned char { Low = 1, High = 2 };  // scoped: type-safe, explicit base

// User-defined literals
constexpr long double operator""_km(long double v) { return v * 1000.0L; }
constexpr unsigned long long operator""_KB(unsigned long long v) { return v * 1024ULL; }
std::string operator""_up(const char* s, std::size_t n) {
    std::string r(s, n);
    for (auto& c : r) c = static_cast<char>(c >= 'a' && c <= 'z' ? c - 32 : c);
    return r;
}

// constexpr: evaluated at compile time when inputs are constant
constexpr unsigned long long Factorial(unsigned n) { return n <= 1 ? 1 : n * Factorial(n - 1); }
static_assert(Factorial(5) == 120, "compile-time factorial");

int main() {
    SECTION("enums");
    CHECK(Green == 5 && Blue == 6);
    int asInt = Blue;  // implicit
    CHECK(asInt == 6);
    Level lv = Level::High;
    CHECK(static_cast<int>(lv) == 2);  // scoped enums need an explicit cast
    CHECK(sizeof(Level) == 1);

    SECTION("raw strings");
    const char* path = R"(C:\temp\new\file.txt)";  // no escaping needed
    CHECK(std::string(path).size() == 20);
    std::string json = R"json({"k": "v"})json";
    CHECK(json.front() == '{');

    SECTION("std::string");
    std::string s = "Hello";
    s += ", World";
    CHECK(s.size() == 12 && s.find("World") == 7);
    CHECK(s.substr(0, 5) == "Hello");
    s.replace(0, 5, "Howdy");
    CHECK(s == "Howdy, World");
    s.insert(5, "!");
    CHECK(s[5] == '!');
    CHECK(std::stoi("123") + 1 == 124 && std::to_string(7) == "7");

    SECTION("string streams");
    std::ostringstream out;
    out << "x=" << 10 << ";y=" << 2.5;
    CHECK(out.str() == "x=10;y=2.5");
    std::istringstream in("alpha 1 beta 2");
    std::string k1, k2; int v1 = 0, v2 = 0;
    in >> k1 >> v1 >> k2 >> v2;
    CHECK(k2 == "beta" && v2 == 2 && v1 == 1);

    SECTION("user-defined literals & constexpr");
    CHECK(2.5_km == 2500.0L);
    CHECK(4_KB == 4096);
    CHECK("shout"_up == "SHOUT");
    constexpr auto f10 = Factorial(10);
    CHECK(f10 == 3628800);
    std::cout << "OK\n";
}
