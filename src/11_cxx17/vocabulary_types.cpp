// Topics (C++17): std::optional, std::variant, std::any, std::string_view
#include "check.hpp"
#include <any>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

std::optional<int> ParseInt(std::string_view s) {
    if (s.empty()) return std::nullopt;
    int v = 0;
    for (char c : s) {
        if (c < '0' || c > '9') return std::nullopt;
        v = v * 10 + (c - '0');
    }
    return v;
}

template <class... Ts> struct Overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;  // deduction guide

// string_view: non-owning, no allocation, O(1) substr
std::size_t CountSpaces(std::string_view sv) {
    std::size_t n = 0;
    for (char c : sv) n += (c == ' ');
    return n;
}

int main() {
    SECTION("std::optional");
    auto good = ParseInt("123");
    auto bad = ParseInt("12x");
    CHECK(good.has_value() && *good == 123);
    CHECK(!bad && bad.value_or(-1) == -1);
    bool threw = false;
    try { (void)bad.value(); } catch (const std::bad_optional_access&) { threw = true; }
    CHECK(threw);
    std::optional<std::string> name;
    name.emplace("Ada");
    CHECK(name->size() == 3);
    name.reset();
    CHECK(!name);

    SECTION("std::variant");
    std::variant<int, double, std::string> v = 42;
    CHECK(v.index() == 0 && std::get<int>(v) == 42);
    v = std::string("text");
    CHECK(std::holds_alternative<std::string>(v));
    CHECK(std::get_if<int>(&v) == nullptr);
    std::string visited = std::visit(Overloaded{
        [](int i) { return "int " + std::to_string(i); },
        [](double d) { return "double " + std::to_string(d); },
        [](const std::string& s) { return "string " + s; }}, v);
    CHECK(visited == "string text");
    bool badAccess = false;
    try { (void)std::get<int>(v); } catch (const std::bad_variant_access&) { badAccess = true; }
    CHECK(badAccess);

    SECTION("std::any");
    std::any a = 10;
    CHECK(a.type() == typeid(int) && std::any_cast<int>(a) == 10);
    a = std::string("now a string");
    CHECK(std::any_cast<std::string>(&a) != nullptr && std::any_cast<int>(&a) == nullptr);
    bool badCast = false;
    try { (void)std::any_cast<int>(a); } catch (const std::bad_any_cast&) { badCast = true; }
    CHECK(badCast);
    a.reset();
    CHECK(!a.has_value());

    SECTION("std::string_view");
    std::string_view sv = "hello big world";
    CHECK(CountSpaces(sv) == 2);
    CHECK(sv.substr(6, 3) == "big");
    sv.remove_prefix(6);
    CHECK(sv.front() == 'b');
    CHECK(sv.find("world") == 4);
    std::cout << "OK\n";
}
