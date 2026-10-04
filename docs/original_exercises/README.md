# Original course exercises

These are my unmodified lecture exercise files, kept for provenance. Each one was later
expanded into a complete, self-checking program under [`src/`](../../src).

| Original | Evolved into | Notes |
|---|---|---|
| `pointers.cpp` | `src/01_basics/03_pointers_references.cpp` | The original dereferences a `nullptr` on purpose to demonstrate **undefined behaviour**; the evolved version guards the dereference. Do not run the original. |
| `swap_function.cpp` | `src/01_basics/03_pointers_references.cpp` | Pointer vs. reference `Swap` overloads, plus a null-safety test. |
| `const_qualifier.cpp` | `src/01_basics/04_const_qualifier.cpp` | `std::cin` replaced by fixed input so it can run as an automated test. |
| `auto_keyword.cpp` | `src/01_basics/05_auto_and_range_for.cpp` | Deduction results are now proven with `static_assert(std::is_same...)`. |
| `parallel.cpp` | `src/11_cxx17/parallel_algorithms.cpp` | Original was missing `<algorithm>`/`<numeric>`, used 100 elements (too small to measure) and had the parallel calls commented out. |
