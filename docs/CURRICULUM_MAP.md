# Curriculum map

Every topic from the course outline and the file that demonstrates it.
Each file is a runnable program that verifies its own behaviour with `CHECK(...)`.

| Course topic | Demonstrated in |
|---|---|
| Primitive types & variables, basic I/O, uniform initialization (C++11) | `src/01_basics/01_types_and_io.cpp` |
| Functions, overloading, default args, inline, function pointers, namespaces | `src/01_basics/02_functions.cpp` |
| Pointers, references, reference vs pointer, swap | `src/01_basics/03_pointers_references.cpp` |
| `const` qualifier & compound types | `src/01_basics/04_const_qualifier.cpp` |
| `auto` type inference, range-based for | `src/01_basics/05_auto_and_range_for.cpp` |
| Dynamic memory: `malloc`, `new`, `new[]`, 2D arrays | `src/02_memory/dynamic_memory.cpp` |
| Class, constructor/destructor, struct, NSDMI, `this`, static members, const member functions, delegating ctors, `= default`/`= delete` | `src/03_oop/01_class_basics.cpp` |
| Copy constructor, l/r-values, move semantics, Rule of 5 & 0, copy elision, `std::move` | `src/03_oop/02_copy_and_move.cpp` |
| Operator overloading (member/global/friend), assignment, smart-pointer basics, type conversions, member init list | `src/03_oop/03_operator_overloading.cpp` |
| Inheritance & composition, access modifiers, inheriting ctors, `virtual`, `override`/`final`, slicing, `typeid`, `dynamic_cast`, abstract classes, diamond inheritance, bank-account project | `src/03_oop/04_inheritance_polymorphism.cpp` |
| Raw pointers, `unique_ptr`, `shared_ptr`, `weak_ptr`, circular references, deleters, dynamic arrays, `make_*` | `src/04_smart_pointers/smart_pointers.cpp` |
| Enums & scoped enums, raw strings, `std::string`, string streams, user-defined literals, `constexpr` | `src/05_language_features/enums_strings_literals.cpp` |
| `std::initializer_list`, `std::vector`, unions | `src/05_language_features/containers_unions_init.cpp` |
| File I/O: text, error handling, copy utility, char I/O & seeking, binary | `src/06_file_io/file_io.cpp` |
| Templates: functions, deduction, specialization, non-type args, perfect forwarding, variadics, class templates, partial specialization, aliases, type traits, `static_assert` | `src/07_templates/templates.cpp` |
| Callbacks, lambdas, captures, generalized capture, `std::function`, `std::bind` | `src/08_lambdas_functional/lambdas_and_functional.cpp` |
| STL sequence/associative/unordered containers, `std::hash`, algorithms, Big-O | `src/09_stl/containers_and_algorithms.cpp` |
| Threads, args, return values, mutex, `lock_guard`, `this_thread`, tasks, launch policies, future waits, promise, cross-thread exceptions | `src/10_concurrency/concurrency.cpp` |
| C++17: attributes, feature-test macros, if/switch init, inline variables, nested namespaces, `noexcept`, constexpr lambda, structured bindings, evaluation order, CTAD, fold expressions, `if constexpr`, trait suffixes | `src/11_cxx17/language_features_17.cpp` (constexpr lambda also in `08_lambdas_functional`) |
| `std::optional`, `std::variant`, `std::any`, `std::string_view` | `src/11_cxx17/vocabulary_types.cpp` |
| `std::filesystem`: path, directory_entry, directory functions, permissions | `src/11_cxx17/filesystem_demo.cpp` |
| Parallel algorithms & execution policies | `src/11_cxx17/parallel_algorithms.cpp` |
