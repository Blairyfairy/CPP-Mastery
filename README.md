# Blair Page: C++ (C++11 → C++17)

[![CI](https://github.com/YOUR-USERNAME/YOUR-REPO-NAME/actions/workflows/ci.yml/badge.svg)](https://github.com/YOUR-USERNAME/YOUR-REPO-NAME/actions)
![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)

A portfolio of **22 runnable, self-verifying C++ programs (~1,800 lines)** covering the full arc of
modern C++: from primitive types and pointers, through OOP, move semantics, templates and the STL,
to concurrency and the C++17 library. Every program asserts its own results, so the repository
doubles as a test suite, and it is built and tested with warnings-as-errors and
AddressSanitizer/UndefinedBehaviorSanitizer.

**Author:** Blair Page

---

## Quick start

```bash
git clone https://github.com/YOUR-USERNAME/YOUR-REPO-NAME.git
cd Blair-Page-C++

# Option A: CMake (recommended; one executable + one CTest test per example)
cmake -S . -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure

# Option B: no CMake needed (g++ or clang++)
./scripts/run_all.sh                       # g++
./scripts/run_all.sh clang++ -O2           # clang++ with extra flags
./scripts/run_all.sh g++ -fsanitize=address,undefined
```

Build options: `-DBP_WARNINGS_AS_ERRORS=ON`, `-DBP_SANITIZE=ON` (ASan + UBSan).
Requires a C++17 compiler (GCC 9+, Clang 10+, MSVC 2019+) and CMake 3.16+ for option A.

Run a single example directly:

```bash
g++ -std=c++17 -Iinclude -pthread src/04_smart_pointers/smart_pointers.cpp -o demo && ./demo
```

## What is covered

| Module | Skills demonstrated |
|---|---|
| **01 Basics** | primitive types, brace initialization, I/O, overloading, default args, `inline`, function pointers, namespaces, pointers vs references, `const` correctness, `auto`, range-based `for` |
| **02 Memory** | `malloc`/`free`, `new`/`delete`, `new[]`/`delete[]`, 2D arrays (row-pointer and flat), `nothrow`, RAII `Matrix` |
| **03 OOP** | classes, constructors/destructors, NSDMI, delegating ctors, `=default`/`=delete`, `this`, static & const members, **Rule of 5/0**, **move semantics**, copy elision, operator overloading, user-defined conversions, inheritance, `virtual`/`override`/`final`, slicing, RTTI, `dynamic_cast`, abstract classes, diamond inheritance, exception hierarchy (bank-account mini project) |
| **04 Smart pointers** | `unique_ptr`, `shared_ptr`, `weak_ptr`, breaking circular references, custom deleters, arrays, `make_unique`/`make_shared` |
| **05 Language features** | scoped enums, raw strings, `std::string`, string streams, user-defined literals, `constexpr`, `initializer_list`, `vector`, tagged unions |
| **06 File I/O** | text and binary streams, error states and exceptions, seeking, a chunked copy utility |
| **07 Templates** | deduction, specialization (full & partial), non-type params, **perfect forwarding**, **variadic templates**, class templates, alias templates, type traits, `static_assert` |
| **08 Lambdas & functional** | callbacks, functors, lambda captures (value/ref/`mutable`/init-capture/`this`), `constexpr` lambdas, `std::function`, `std::bind` |
| **09 STL** | all sequence, ordered and unordered containers, custom `std::hash`, algorithms (erase-remove idiom, `transform`, `accumulate`, `minmax_element`...), complexity notes |
| **10 Concurrency** | `std::thread`, `std::ref`, `mutex` + `lock_guard`, `std::async`, launch policies, `future` waits, `promise`, exception propagation across threads |
| **11 C++17** | structured bindings, CTAD, fold expressions, `if constexpr`, `if`/`switch` initializers, inline variables, nested namespaces, `[[nodiscard]]`, `optional`, `variant` + `visit`, `any`, `string_view`, `<filesystem>`, parallel algorithms with a benchmark timer |

A topic-by-topic index of the full course outline lives in [`docs/CURRICULUM_MAP.md`](docs/CURRICULUM_MAP.md).

## Repository layout

```
Blair-Page-C++/
├── CMakeLists.txt            # globs src/*/*.cpp -> one target + one test each
├── include/check.hpp         # tiny CHECK()/SECTION() test helpers
├── src/
│   ├── 01_basics/            ├── 02_memory/          ├── 03_oop/
│   ├── 04_smart_pointers/    ├── 05_language_features/
│   ├── 06_file_io/           ├── 07_templates/       ├── 08_lambdas_functional/
│   └── 09_stl/               ├── 10_concurrency/     └── 11_cxx17/
├── docs/
│   ├── CURRICULUM_MAP.md
│   └── original_exercises/   # my original lecture files, kept for provenance
├── scripts/run_all.sh        # build + run everything without CMake
└── .github/workflows/ci.yml  # Linux (sanitizers) / Windows / macOS
```

## Engineering practices shown

- **Self-verifying examples:** each file asserts expected behaviour (including failure paths such as
  exceptions, failed `dynamic_cast`, bad `optional`/`variant`/`any` access, and missing files).
- **Compile-time checks:** `static_assert` with type traits to prove `auto` deduction, template
  specialization choice, and `constexpr` evaluation.
- **Resource safety:** RAII throughout; ownership expressed with smart pointers; leak/lifetime
  counters in tests (e.g. proving that `weak_ptr` breaks reference cycles).
- **Quality gates:** `-Wall -Wextra -Wpedantic`, optional `-Werror`, ASan + UBSan, and a CI matrix
  across three operating systems.
- **Determinism:** no interactive input, fixed RNG seeds, temp-directory I/O that cleans up.

## Notes

- `parallel_algorithms.cpp` uses `std::execution` policies when Parallel STL is available
  (MSVC out of the box; GCC/Clang need Intel TBB, which CMake links automatically if found).
  Without it, the program still runs the sequential baselines and skips the parallel section.
- Timing output is machine-dependent and printed for comparison only; it is never asserted.
- `docs/original_exercises/` contains the raw lecture exercises. `pointers.cpp` intentionally
  triggers undefined behaviour (null dereference) as a teaching demo and is **not** built.

## License

MIT, see [LICENSE](LICENSE).
