// Topics: raw pointers, std::unique_ptr, sharing pointers, std::shared_ptr, weak ownership
//         (std::weak_ptr), circular references, custom deleters, dynamic arrays, make functions
#include "check.hpp"
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

struct Tracked {
    static int alive;
    Tracked() { ++alive; }
    ~Tracked() { --alive; }
};
int Tracked::alive = 0;

// Circular reference: with two shared_ptr owners these would leak.
struct Node {
    static int alive;
    std::shared_ptr<Node> next;
    std::weak_ptr<Node> prev;  // weak back-pointer breaks the cycle
    Node() { ++alive; }
    ~Node() { --alive; }
};
int Node::alive = 0;

std::unique_ptr<Tracked> Factory() { return std::make_unique<Tracked>(); }  // ownership out
void Sink(std::unique_ptr<Tracked> p) { (void)p; }                          // ownership in

int main() {
    SECTION("std::unique_ptr: exclusive, move-only ownership");
    {
        auto p = std::make_unique<Tracked>();
        CHECK(Tracked::alive == 1);
        // auto q = p;               // error: not copyable
        auto q = std::move(p);       // transfer
        CHECK(p == nullptr && q != nullptr);
        auto r = Factory();
        CHECK(Tracked::alive == 2);
        Sink(std::move(r));          // destroyed when Sink returns
        CHECK(Tracked::alive == 1);
    }
    CHECK(Tracked::alive == 0);

    SECTION("std::shared_ptr: reference counted");
    {
        auto a = std::make_shared<Tracked>();
        CHECK(a.use_count() == 1);
        {
            auto b = a;
            CHECK(a.use_count() == 2);
        }
        CHECK(a.use_count() == 1);
    }
    CHECK(Tracked::alive == 0);

    SECTION("std::weak_ptr: non-owning observer");
    std::weak_ptr<Tracked> w;
    {
        auto sp = std::make_shared<Tracked>();
        w = sp;
        CHECK(!w.expired() && w.use_count() == 1);
        if (auto locked = w.lock()) CHECK(locked.use_count() == 2);
    }
    CHECK(w.expired() && w.lock() == nullptr);

    SECTION("circular references solved with weak_ptr");
    {
        auto n1 = std::make_shared<Node>();
        auto n2 = std::make_shared<Node>();
        n1->next = n2;
        n2->prev = n1;  // weak => no cycle of owners
        CHECK(Node::alive == 2);
    }
    CHECK(Node::alive == 0);  // would be 2 (leak) if prev were a shared_ptr

    SECTION("custom deleters");
    static int closed = 0;
    {
        auto closer = [](std::FILE* f) { if (f) { std::fclose(f); ++closed; } };
        std::unique_ptr<std::FILE, decltype(closer)> file{std::tmpfile(), closer};
        CHECK(file != nullptr);
    }
    CHECK(closed == 1);
    {
        std::shared_ptr<int> sp{new int[3]{1, 2, 3}, [](int* p) { delete[] p; }};
        CHECK(sp.use_count() == 1);
    }

    SECTION("dynamic arrays & make functions");
    std::unique_ptr<int[]> arr{new int[4]{1, 2, 3, 4}};  // uses delete[] automatically
    CHECK(arr[3] == 4);
    auto s = std::make_shared<std::string>("hi");  // single allocation for object + control block
    CHECK(*s == "hi");
    std::cout << "OK\n";
}
