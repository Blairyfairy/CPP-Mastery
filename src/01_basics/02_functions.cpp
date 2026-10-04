// Topics: function basics, overloading, default arguments, inline functions,
//         function pointers, namespaces
#include "check.hpp"
#include <string>

// ---- overloading: same name, different parameter lists
int Sum(int x, int y) { return x + y; }
double Sum(double x, double y) { return x + y; }
std::string Sum(const std::string& a, const std::string& b) { return a + b; }

// ---- default arguments (right-most parameters first)
int Volume(int length, int width = 1, int height = 1) { return length * width * height; }

// ---- inline: removes call overhead and relaxes the ODR for header definitions
inline int Square(int x) { return x * x; }

// ---- function pointers
using BinaryOp = int (*)(int, int);
int Add(int a, int b) { return a + b; }
int Mul(int a, int b) { return a * b; }
int Apply(BinaryOp op, int a, int b) { return op(a, b); }

// ---- namespaces
namespace geometry {
constexpr double kPi = 3.14159265358979;
namespace shapes {
double CircleArea(double r) { return kPi * r * r; }
}  // namespace shapes
}  // namespace geometry
namespace {  // anonymous namespace: internal linkage
int hidden_counter = 0;
}
namespace gs = geometry::shapes;  // namespace alias

int main() {
    SECTION("overloading");
    CHECK(Sum(2, 3) == 5);
    CHECK(Sum(2.5, 0.5) == 3.0);
    CHECK(Sum(std::string("foo"), std::string("bar")) == "foobar");

    SECTION("default arguments");
    CHECK(Volume(2) == 2);
    CHECK(Volume(2, 3) == 6);
    CHECK(Volume(2, 3, 4) == 24);

    SECTION("inline");
    CHECK(Square(9) == 81);

    SECTION("function pointers");
    BinaryOp op = Add;
    CHECK(Apply(op, 3, 4) == 7);
    op = Mul;
    CHECK(Apply(op, 3, 4) == 12);

    SECTION("namespaces");
    ++hidden_counter;
    CHECK(hidden_counter == 1);
    double area = gs::CircleArea(2.0);
    std::cout << "circle area r=2: " << area << "\n";
    CHECK(area > 12.56 && area < 12.57);
    std::cout << "OK\n";
}
