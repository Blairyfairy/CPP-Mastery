// Topics: dynamic memory - malloc/free, new/delete, new[]/delete[], 2D arrays, RAII
#include "check.hpp"
#include <cstdlib>
#include <new>
#include <string>

struct Tracker {
    static int alive;
    Tracker() { ++alive; }
    ~Tracker() { --alive; }
};
int Tracker::alive = 0;

// Flat, contiguous 2D array wrapper: one allocation, cache-friendly, freed automatically.
class Matrix {
    int rows_, cols_;
    int* data_;

public:
    Matrix(int r, int c) : rows_{r}, cols_{c}, data_{new int[r * c]{}} {}
    ~Matrix() { delete[] data_; }
    Matrix(const Matrix&) = delete;
    Matrix& operator=(const Matrix&) = delete;
    int& at(int r, int c) { return data_[r * cols_ + c]; }
};

int main() {
    SECTION("malloc / free (C style: raw bytes, no constructors)");
    int* m = static_cast<int*>(std::malloc(4 * sizeof(int)));
    CHECK(m != nullptr);
    for (int i = 0; i < 4; ++i) m[i] = i * i;
    CHECK(m[3] == 9);
    std::free(m);

    SECTION("new / delete (allocates AND constructs)");
    CHECK(Tracker::alive == 0);
    Tracker* t = new Tracker;
    CHECK(Tracker::alive == 1);
    delete t;  // runs the destructor
    CHECK(Tracker::alive == 0);
    int* n = new int{42};
    CHECK(*n == 42);
    delete n;

    SECTION("new[] / delete[]");
    Tracker* arr = new Tracker[3];
    CHECK(Tracker::alive == 3);
    delete[] arr;  // must match new[]
    CHECK(Tracker::alive == 0);
    int* zeros = new int[5]();  // () value-initializes to zero
    CHECK(zeros[4] == 0);
    delete[] zeros;

    SECTION("nothrow new");
    int* p = new (std::nothrow) int[10];
    CHECK(p != nullptr);
    delete[] p;

    SECTION("2D array: array of row pointers");
    const int R = 3, C = 4;
    int** grid = new int*[R];
    for (int r = 0; r < R; ++r) grid[r] = new int[C]{};
    grid[2][3] = 7;
    CHECK(grid[2][3] == 7);
    for (int r = 0; r < R; ++r) delete[] grid[r];
    delete[] grid;

    SECTION("2D array: flat RAII Matrix");
    Matrix mat(3, 4);
    mat.at(1, 2) = 5;
    CHECK(mat.at(1, 2) == 5 && mat.at(0, 0) == 0);
    std::cout << "OK\n";
}
