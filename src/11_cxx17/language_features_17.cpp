// Topics (C++17): attributes, feature-test macros, if/switch with initializer, inline variables,
//   nested namespaces, noexcept, structured bindings, expression evaluation order,
//   class template argument deduction (CTAD), fold expressions, if constexpr, type-trait suffixes
#include "check.hpp"
#include <map>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#if defined(__has_cpp_attribute) && __has_cpp_attribute(nodiscard)
#define HAVE_NODISCARD 1
#endif

// ---- attributes
[[nodiscard]] int Compute() { return 42; }
[[noreturn]] void Fail() { throw 1; }
[[deprecated("use Compute")]] inline int OldCompute() { return 42; }

// ---- inline variables (a header-safe global)
inline int g_counter = 0;

// ---- nested namespace definition
namespace company::product::v1 {
constexpr int kVersion = 1;
}

// ---- noexcept as specifier and operator
int Safe() noexcept { return 1; }
int Risky() { return 2; }
static_assert(noexcept(Safe()) && !noexcept(Risky()), "noexcept operator");

// ---- structured bindings
struct Point { int x, y; };
std::pair<int, std::string> Lookup() { return {7, "seven"}; }

// ---- CTAD
template <typename T>
struct Box {
    T value;
    Box(T v) : value{std::move(v)} {}
};

// ---- fold expressions
template <typename... Ts> auto All(Ts... ts) { return (... && ts); }
template <typename... Ts> auto AddAll(Ts... ts) { return (ts + ... + 0); }  // binary fold with init
template <typename... Ts> std::string Join(const Ts&... ts) {
    std::string out;
    ((out += ts, out += ','), ...);  // comma fold
    return out;
}

// ---- if constexpr: branches are discarded at compile time
template <typename T>
std::string Kind(const T& v) {
    if constexpr (std::is_integral_v<T>) return "int:" + std::to_string(v);
    else if constexpr (std::is_same_v<T, std::string>) return "str:" + v;
    else return "other";
}

int main() {
    SECTION("attributes & feature-test macros");
    int r = Compute();  // ignoring the result of a [[nodiscard]] function warns
    CHECK(r == 42);
#ifdef HAVE_NODISCARD
    std::cout << "nodiscard supported\n";
#endif
#if __cplusplus >= 201703L
    std::cout << "compiled as C++17 or newer\n";
#endif
    if (false) Fail();

    SECTION("if / switch with initializer");
    std::map<std::string, int> m{{"k", 1}};
    if (auto it = m.find("k"); it != m.end()) CHECK(it->second == 1);
    switch (int n = Safe(); n) { case 1: break; default: CHECK(false); }

    SECTION("inline variable, nested namespace");
    ++g_counter;
    CHECK(g_counter == 1 && company::product::v1::kVersion == 1);

    SECTION("structured bindings");
    auto [num, word] = Lookup();
    CHECK(num == 7 && word == "seven");
    Point p{3, 4};
    auto& [px, py] = p;  // binds by reference
    px = 30;
    CHECK(p.x == 30 && py == 4);
    for (const auto& [k, v] : m) CHECK(k == "k" && v == 1);
    auto [a, b, c] = std::make_tuple(1, 2.5, 'x');
    CHECK(a == 1 && b == 2.5 && c == 'x');

    SECTION("evaluation order (C++17 guarantees)");
    std::string s = "I heard it";
    s.replace(0, 1, "We").replace(s.find("heard"), 5, "saw");  // left-to-right now guaranteed
    std::cout << s << "\n";

    SECTION("CTAD");
    Box b1{5};                       // Box<int>
    Box b2{std::string("hi")};       // Box<std::string>
    std::vector v{1, 2, 3};          // vector<int>
    std::pair pr{1, 2.0};            // pair<int,double>
    static_assert(std::is_same_v<decltype(b1), Box<int>>);
    CHECK(b2.value == "hi" && v.size() == 3 && pr.second == 2.0);

    SECTION("fold expressions");
    CHECK(All(true, true, false) == false && All() == true);
    CHECK(AddAll(1, 2, 3, 4) == 10);
    CHECK(Join("a", "b", "c") == "a,b,c,");

    SECTION("if constexpr & trait suffixes");
    CHECK(Kind(5) == "int:5" && Kind(std::string("x")) == "str:x" && Kind(2.5) == "other");
    static_assert(std::is_same_v<std::decay_t<const int&>, int>);
    std::cout << "OK\n";
}
