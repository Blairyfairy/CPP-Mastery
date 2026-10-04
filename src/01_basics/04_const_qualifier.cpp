// Topics: const qualifier, const with pointers (compound types), const references
#include "check.hpp"

int Read(const int* ptr) { return *ptr; }    // promise not to modify the pointee
int ReadRef(const int& ref) { return ref; }  // binds to lvalues AND temporaries

int main() {
    SECTION("const variables");
    const float PI = 3.14159f;
    float radius = 2.0f;
    float area = PI * radius * radius;
    float circumference = PI * 2 * radius;
    std::cout << "area=" << area << " circumference=" << circumference << "\n";
    CHECK(area > 12.5f && area < 12.6f);
    // PI = 3;  // error: assignment of read-only variable

    SECTION("const and pointers");
    int a = 1, b = 2;
    const int* p1 = &a;  // pointer to const int: data read-only, pointer re-assignable
    p1 = &b;
    // *p1 = 5;          // error
    int* const p2 = &a;  // const pointer to int: data writable, pointer fixed
    *p2 = 7;
    // p2 = &b;          // error
    const int* const p3 = &a;  // both fixed
    CHECK(*p1 == 2 && a == 7 && *p3 == 7);
    const int CHUNK = 512;
    const int* pc = &CHUNK;  // a plain int* would not compile: it would drop const
    CHECK(Read(pc) == 512);

    SECTION("const references");
    CHECK(ReadRef(1) == 1);  // a temporary binds to const&
    CHECK(ReadRef(a) == 7);
    // int& bad = 1;         // error: non-const lvalue ref cannot bind to a temporary
    std::cout << "OK\n";
}
