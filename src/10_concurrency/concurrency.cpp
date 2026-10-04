// Topics: std::thread, passing args, returning values, std::mutex, std::lock_guard,
//         std::this_thread, task-based concurrency (std::async), launch policies,
//         std::future wait functions, std::promise, propagating exceptions across threads
#include "check.hpp"
#include <chrono>
#include <future>
#include <mutex>
#include <numeric>
#include <stdexcept>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

void AddTo(int& target, int amount) { target += amount; }

int SlowSquare(int x) {
    std::this_thread::sleep_for(20ms);
    return x * x;
}
int Throws() { throw std::runtime_error("boom"); }

int main() {
    SECTION("thread creation & argument passing");
    int value = 0;
    std::thread t1{AddTo, std::ref(value), 5};  // std::ref needed to pass by reference
    t1.join();                                    // always join or detach before destruction
    CHECK(value == 5);
    std::cout << "hardware threads: " << std::thread::hardware_concurrency() << "\n";  // 0 = unknown
    std::thread t2{[] { std::this_thread::yield(); }};
    CHECK(t2.joinable());
    t2.join();
    CHECK(!t2.joinable());

    SECTION("data race prevention: mutex + lock_guard");
    long counter = 0;
    std::mutex mtx;
    auto work = [&] {
        for (int i = 0; i < 10000; ++i) {
            std::lock_guard<std::mutex> lock{mtx};  // RAII unlock, exception-safe
            ++counter;
        }
    };
    std::vector<std::thread> pool;
    for (int i = 0; i < 4; ++i) pool.emplace_back(work);
    for (auto& t : pool) t.join();
    CHECK(counter == 40000);

    SECTION("returning values: std::async / std::future");
    auto fut = std::async(std::launch::async, SlowSquare, 9);
    CHECK(fut.get() == 81);  // blocks until ready; can be called once

    SECTION("launch policies");
    auto deferred = std::async(std::launch::deferred, [] { return std::this_thread::get_id(); });
    CHECK(deferred.get() == std::this_thread::get_id());  // deferred => runs in the caller on get()
    auto asyncId = std::async(std::launch::async, [] { return std::this_thread::get_id(); });
    CHECK(asyncId.get() != std::this_thread::get_id());   // async => new thread

    SECTION("future wait functions");
    auto slow = std::async(std::launch::async, SlowSquare, 3);
    auto status = slow.wait_for(0ms);
    CHECK(status == std::future_status::timeout || status == std::future_status::ready);
    slow.wait();
    CHECK(slow.wait_for(0ms) == std::future_status::ready);
    CHECK(slow.get() == 9);

    SECTION("std::promise");
    std::promise<int> prom;
    std::future<int> pf = prom.get_future();
    std::thread producer{[&prom] { std::this_thread::sleep_for(10ms); prom.set_value(123); }};
    CHECK(pf.get() == 123);
    producer.join();

    SECTION("propagating exceptions across threads");
    auto bad = std::async(std::launch::async, Throws);
    bool caught = false;
    try { bad.get(); } catch (const std::runtime_error& e) { caught = std::string(e.what()) == "boom"; }
    CHECK(caught);
    std::promise<int> p2;
    auto f2 = p2.get_future();
    std::thread th{[&p2] {
        try { throw std::logic_error("via promise"); }
        catch (...) { p2.set_exception(std::current_exception()); }
    }};
    bool caught2 = false;
    try { f2.get(); } catch (const std::logic_error&) { caught2 = true; }
    th.join();
    CHECK(caught2);
    std::cout << "OK\n";
}
