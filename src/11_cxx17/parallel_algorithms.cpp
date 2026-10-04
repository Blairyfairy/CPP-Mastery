// Topics (C++17): parallel algorithms & execution policies, with a small RAII benchmark timer.
// Evolved from the original course exercise "parallel.cpp" (missing includes, tiny data set,
// and execution-policy lines commented out) into a verified, measurable comparison.
#include "check.hpp"
#include <algorithm>
#include <chrono>
#include <numeric>
#include <random>
#include <string_view>
#include <vector>
#if defined(BP_HAVE_PARALLEL_STL)
#include <execution>
#endif

class Timer {
    std::chrono::steady_clock::time_point start_{std::chrono::steady_clock::now()};

public:
    long long ElapsedMicros() const {
        return std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::steady_clock::now() - start_).count();
    }
    void Show(std::string_view label) const { std::cout << label << ": " << ElapsedMicros() << " us\n"; }
};

constexpr std::size_t kSize = 2'000'000;

std::vector<long> CreateVector(std::size_t n) {
    std::vector<long> vec;
    vec.reserve(n);
    std::mt19937_64 engine{12345};  // fixed seed => reproducible
    std::uniform_int_distribution<long> dist{0, 1'000'000};
    for (std::size_t i = 0; i < n; ++i) vec.push_back(dist(engine));
    return vec;
}

int main() {
    const auto data = CreateVector(kSize);
    SECTION("sequential baselines");
    auto seq = data;
    Timer t1;
    std::sort(seq.begin(), seq.end());
    t1.Show("std::sort (sequential)");
    CHECK(std::is_sorted(seq.begin(), seq.end()));
    Timer t2;
    long accSum = std::accumulate(data.begin(), data.end(), 0L);  // strictly left-to-right
    t2.Show("std::accumulate");
    Timer t3;
    long redSum = std::reduce(data.begin(), data.end(), 0L);  // may reorder: needs associativity
    t3.Show("std::reduce (no policy)");
    CHECK(accSum == redSum);

#if defined(BP_HAVE_PARALLEL_STL)
    SECTION("parallel execution policies");
    auto par = data;
    Timer t4;
    std::sort(std::execution::par, par.begin(), par.end());
    t4.Show("std::sort (par)");
    CHECK(par == seq);
    Timer t5;
    long parSum = std::reduce(std::execution::par, data.begin(), data.end(), 0L);
    t5.Show("std::reduce (par)");
    CHECK(parSum == accSum);
    long countEven = std::count_if(std::execution::par_unseq, data.begin(), data.end(),
                                   [](long v) { return v % 2 == 0; });
    CHECK(countEven > 0);
    std::vector<long> doubled(data.size());
    std::transform(std::execution::par, data.begin(), data.end(), doubled.begin(),
                   [](long v) { return v * 2; });
    CHECK(doubled[10] == data[10] * 2);
#else
    std::cout << "Parallel STL (TBB) not available on this toolchain: skipping par policies.\n";
#endif
    std::cout << "OK\n";
}
