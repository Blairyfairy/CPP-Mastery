// Topics: automatic type inference (C++11), range-based for loop (C++11)
#include "check.hpp"
#include <map>
#include <string>
#include <type_traits>
#include <vector>

int Sum(int x, int y) { return x + y; }

int main() {
    SECTION("auto deduction");
    auto i = 10;              // int
    auto sum = i + 4.3f;      // float (usual arithmetic conversions)
    auto result = Sum(i, 5);  // int, from the return type
    static auto s = 2;        // works with storage specifiers
    const int x = 10;
    const auto var = x;  // qualifiers can be added
    auto& var1 = x;      // deduced as const int&
    auto* ptr = &x;      // deduced as const int*
    static_assert(std::is_same<decltype(sum), float>::value, "float expected");
    static_assert(std::is_same<decltype(var1), const int&>::value, "const int& expected");
    static_assert(std::is_same<decltype(ptr), const int*>::value, "const int* expected");
    CHECK(result == 15 && s == 2 && var == 10 && *ptr == 10);
    auto copy = var1;  // auto drops references and top-level const: plain int
    static_assert(std::is_same<decltype(copy), int>::value, "int expected");
    (void)copy;

    SECTION("range-based for");
    int arr[]{1, 2, 3, 4};
    int total = 0;
    for (auto v : arr) total += v;  // by value (copy)
    CHECK(total == 10);

    std::vector<int> vec{1, 2, 3};
    for (auto& v : vec) v *= 2;  // by reference to modify in place
    CHECK(vec[2] == 6);
    for (const auto& v : vec) std::cout << v << ' ';  // const& avoids copies of big types
    std::cout << "\n";

    std::map<std::string, int> ages{{"ann", 30}, {"bob", 25}};
    for (const auto& kv : ages) std::cout << kv.first << "=" << kv.second << "\n";
    for (auto c : {'a', 'b', 'c'}) std::cout << c;  // braced-init-list works too
    std::cout << "\nOK\n";
}
