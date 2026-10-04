// Topics: primitive types & variables, basic I/O, uniform initialization (C++11)
#include "check.hpp"
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>

int main() {
    SECTION("primitive types");
    int i = 42;
    unsigned u = 7u;
    long long big = 1LL << 40;
    float f = 3.14f;
    double d = 2.718281828;
    char c = 'A';
    bool b = true;
    std::cout << "int=" << i << " unsigned=" << u << " long long=" << big << " float=" << f
              << " double=" << d << " char=" << c << " bool=" << std::boolalpha << b << "\n";
    std::cout << "sizeof(int)=" << sizeof(int) << " sizeof(long long)=" << sizeof(long long)
              << " INT_MAX=" << std::numeric_limits<int>::max() << "\n";
    CHECK(sizeof(char) == 1);
    CHECK(sizeof(long long) >= 8);
    CHECK(static_cast<int>(c) == 65);

    SECTION("uniform (brace) initialization");
    int x{5};
    double y{2.5};
    int zero{};  // value-initialized to 0
    int arr[]{1, 2, 3};
    // int narrow{3.5};  // ill-formed: narrowing conversions are rejected by {}
    CHECK(x == 5 && y == 2.5 && zero == 0 && arr[2] == 3);

    SECTION("basic I/O (istringstream stands in for std::cin so the test is deterministic)");
    std::istringstream in("12 3.5 hello");
    int a = 0;
    double r = 0;
    std::string word;
    in >> a >> r >> word;
    CHECK(a == 12 && r == 3.5 && word == "hello");
    std::cout << std::fixed << std::setprecision(2) << "parsed: " << a << ", " << r << ", "
              << word << "\n";
    std::cout << "OK\n";
}
