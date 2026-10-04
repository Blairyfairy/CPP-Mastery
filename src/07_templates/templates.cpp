// Topics: function templates, argument deduction & instantiation, explicit specialization,
//         non-type template arguments, perfect forwarding, variadic templates, class templates,
//         class template specialization (explicit & partial), type aliases / alias templates,
//         type traits, static_assert
#include "check.hpp"
#include <cstring>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// ---- function template + deduction
template <typename T>
T Max(T a, T b) { return a > b ? a : b; }

// ---- explicit (full) specialization
template <>
const char* Max<const char*>(const char* a, const char* b) { return std::strcmp(a, b) > 0 ? a : b; }

// ---- non-type template argument
template <typename T, std::size_t N>
constexpr std::size_t ArraySize(const T (&)[N]) { return N; }

// ---- perfect forwarding: preserves value category
struct Probe {
    std::string kind;
    Probe(const std::string&) : kind{"copy"} {}
    Probe(std::string&&) : kind{"move"} {}
};
template <typename T, typename Arg>
std::unique_ptr<T> Make(Arg&& arg) { return std::make_unique<T>(std::forward<Arg>(arg)); }

// ---- variadic templates (recursion + C++17 fold)
template <typename T>
T SumRec(T v) { return v; }
template <typename T, typename... Rest>
T SumRec(T first, Rest... rest) { return first + SumRec(rest...); }
template <typename... Args>
auto SumFold(Args... args) { return (args + ...); }
template <typename... Args>
std::size_t Count(Args&&...) { return sizeof...(Args); }

// ---- class template
template <typename T>
class Stack {
    std::vector<T> items_;

public:
    void Push(T v) { items_.push_back(std::move(v)); }
    T Pop() { T v = std::move(items_.back()); items_.pop_back(); return v; }
    bool Empty() const { return items_.empty(); }
    std::size_t Size() const { return items_.size(); }
};
// explicit specialization of the class for bool: stored as bits
template <>
class Stack<bool> {
    unsigned bits_{0};
    unsigned n_{0};

public:
    void Push(bool b) { bits_ = (bits_ << 1) | (b ? 1u : 0u); ++n_; }
    bool Pop() { bool b = bits_ & 1u; bits_ >>= 1; --n_; return b; }
    std::size_t Size() const { return n_; }
    static constexpr const char* kind = "bitpacked";
};

// ---- partial specialization
template <typename T, typename U>
struct Pair { static constexpr const char* kind = "generic"; };
template <typename T>
struct Pair<T, T> { static constexpr const char* kind = "same-type"; };
template <typename T, typename U>
struct Pair<T*, U*> { static constexpr const char* kind = "pointers"; };

// ---- alias templates
template <typename T>
using Table = std::vector<std::vector<T>>;
using Id = unsigned long;  // typedef-style alias

// ---- type traits + static_assert
template <typename T>
T Half(T v) {
    static_assert(std::is_arithmetic<T>::value, "Half requires an arithmetic type");
    return v / 2;
}
template <typename T>
std::string Describe() {
    if (std::is_integral<T>::value) return "integral";
    if (std::is_floating_point<T>::value) return "floating";
    return "other";
}

int main() {
    SECTION("function templates, deduction & specialization");
    CHECK(Max(3, 7) == 7);
    CHECK(Max(2.5, 1.5) == 2.5);
    CHECK(Max<double>(1, 2.5) == 2.5);  // explicit argument
    CHECK(std::string(Max("apple", "pear")) == "pear");

    SECTION("non-type arguments");
    int arr[7]{};
    static_assert(ArraySize(arr) == 7, "compile-time size");
    CHECK(ArraySize(arr) == 7);

    SECTION("perfect forwarding");
    std::string s = "lvalue";
    CHECK(Make<Probe>(s)->kind == "copy");
    CHECK(Make<Probe>(std::string("rvalue"))->kind == "move");

    SECTION("variadic templates");
    CHECK(SumRec(1, 2, 3, 4) == 10);
    CHECK(SumFold(1.5, 2.5, 3.0) == 7.0);
    CHECK(Count(1, "a", 2.0, 'c') == 4);

    SECTION("class templates & specializations");
    Stack<int> st;
    st.Push(1); st.Push(2);
    CHECK(st.Pop() == 2 && st.Size() == 1);
    Stack<std::string> ss;
    ss.Push("x");
    CHECK(ss.Pop() == "x" && ss.Empty());
    Stack<bool> sb;
    sb.Push(true); sb.Push(false);
    CHECK(sb.Pop() == false && sb.Pop() == true);
    CHECK(std::string(Stack<bool>::kind) == "bitpacked");
    CHECK(std::string(Pair<int, double>::kind) == "generic");
    CHECK(std::string(Pair<int, int>::kind) == "same-type");
    CHECK(std::string(Pair<int*, char*>::kind) == "pointers");

    SECTION("aliases, type traits & static_assert");
    Table<int> t{{1, 2}, {3, 4}};
    CHECK(t[1][0] == 3);
    Id id = 5;
    CHECK(id == 5);
    CHECK(Half(9) == 4 && Half(9.0) == 4.5);
    CHECK(Describe<int>() == "integral" && Describe<float>() == "floating" &&
          Describe<std::string>() == "other");
    static_assert(std::is_same_v<std::remove_reference_t<int&>, int>, "trait suffix _t / _v");
    // Half(std::string{});  // compile error from static_assert
    std::cout << "OK\n";
}
