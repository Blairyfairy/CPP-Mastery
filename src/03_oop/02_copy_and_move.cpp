// Topics: copy constructor, l-values/r-values, move semantics (C++11), Rule of 5 & 0,
//         copy elision (mandatory in C++17), std::move
#include "check.hpp"
#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

// Rule of 5: owning raw memory => define all five special member functions.
class Buffer {
    std::size_t size_{0};
    int* data_{nullptr};

public:
    static int copies, moves;
    explicit Buffer(std::size_t n) : size_{n}, data_{new int[n]{}} {}
    ~Buffer() { delete[] data_; }
    Buffer(const Buffer& o) : size_{o.size_}, data_{new int[o.size_]} {  // deep copy
        std::copy(o.data_, o.data_ + size_, data_);
        ++copies;
    }
    Buffer(Buffer&& o) noexcept : size_{o.size_}, data_{o.data_} {  // steal resources
        o.size_ = 0;
        o.data_ = nullptr;
        ++moves;
    }
    Buffer& operator=(const Buffer& o) {
        if (this != &o) {
            Buffer tmp{o};  // copy-and-swap style
            std::swap(size_, tmp.size_);
            std::swap(data_, tmp.data_);
        }
        return *this;
    }
    Buffer& operator=(Buffer&& o) noexcept {
        if (this != &o) {
            delete[] data_;
            size_ = o.size_;
            data_ = o.data_;
            o.size_ = 0;
            o.data_ = nullptr;
        }
        return *this;
    }
    std::size_t size() const { return size_; }
    int& operator[](std::size_t i) { return data_[i]; }
};
int Buffer::copies = 0;
int Buffer::moves = 0;

// Rule of 0: let members (unique_ptr, vector) manage themselves; write no special members.
struct Session {
    std::unique_ptr<int> token{std::make_unique<int>(7)};
    std::vector<int> history{1, 2, 3};
};

// C++17 guaranteed elision: works even though the type is neither copyable nor movable.
struct Pinned {
    Pinned() = default;
    Pinned(const Pinned&) = delete;
    Pinned(Pinned&&) = delete;
};
Pinned MakePinned() { return Pinned{}; }

Buffer MakeBuffer() { return Buffer{16}; }

void TakeByValue(Buffer b) { (void)b; }

int main() {
    SECTION("lvalues vs rvalues");
    int x = 5;          // x is an lvalue (has identity)
    int&& r = x + 1;    // x + 1 is an rvalue; an rvalue reference may bind to it
    CHECK(r == 6);
    // int&& bad = x;   // error: cannot bind an rvalue reference to an lvalue

    SECTION("copy constructor");
    Buffer a{8};
    a[0] = 42;
    Buffer b{a};  // deep copy
    b[0] = 1;
    CHECK(a[0] == 42 && Buffer::copies == 1);

    SECTION("move constructor & std::move");
    Buffer c{std::move(a)};  // a is now in a valid-but-unspecified (empty) state
    CHECK(c[0] == 42 && a.size() == 0 && Buffer::moves == 1);
    TakeByValue(std::move(c));
    CHECK(Buffer::moves == 2);

    SECTION("move assignment");
    Buffer d{1};
    d = MakeBuffer();  // prvalue => move assignment
    CHECK(d.size() == 16);

    SECTION("copy elision");
    int movesBefore = Buffer::moves, copiesBefore = Buffer::copies;
    Buffer e = MakeBuffer();  // no copy, no move: constructed in place
    CHECK(Buffer::moves == movesBefore && Buffer::copies == copiesBefore);
    Pinned p = MakePinned();  // compiles only because of mandatory elision
    (void)p; (void)e;

    SECTION("Rule of 0");
    Session s1;
    Session s2{std::move(s1)};  // implicitly-generated move works
    CHECK(*s2.token == 7 && s1.token == nullptr);
    std::cout << "OK\n";
}
