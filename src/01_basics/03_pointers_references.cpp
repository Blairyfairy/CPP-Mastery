// Topics: pointers, references, reference vs pointer, swap (overloaded for both)
#include "check.hpp"

// Pointer version: caller passes addresses, and a null check is required.
void Swap(int* x, int* y) {
    if (!x || !y) return;
    int temp = *x;
    *x = *y;
    *y = temp;
}
// Reference version: cleaner call site, can never be null.
void Swap(int& x, int& y) {
    int temp = x;
    x = y;
    y = temp;
}

int main() {
    SECTION("pointers");
    int x = 10;
    int* ptr = nullptr;  // always initialize pointers (to nullptr if nothing else)
    if (ptr) std::cout << *ptr << "\n";  // dereferencing nullptr is UB, so guard first
    ptr = &x;
    *ptr = 5;
    CHECK(x == 5);
    int arr[]{10, 20, 30};
    int* p = arr;  // arrays decay to pointers; arithmetic is scaled by sizeof(int)
    CHECK(*(p + 2) == 30);
    CHECK((p + 2) - p == 2);

    SECTION("references");
    int& ref = x;  // must be initialized, cannot be reseated, no null reference
    ref = 99;
    CHECK(x == 99);
    int y = 1;
    ref = y;  // assigns the VALUE of y to x; ref still refers to x
    CHECK(x == 1 && &ref == &x);

    SECTION("swap: pointer vs reference");
    int a = 5, b = 10;
    Swap(a, b);
    CHECK(a == 10 && b == 5);
    Swap(&a, &b);
    CHECK(a == 5 && b == 10);
    Swap(nullptr, &b);  // safe because of the null check
    CHECK(b == 10);
    std::cout << "OK\n";
}
