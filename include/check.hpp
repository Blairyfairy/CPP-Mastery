#pragma once
// Tiny self-checking helpers so every example doubles as a test.
#include <cstdlib>
#include <iostream>

#define CHECK(cond)                                                          \
    do {                                                                     \
        if (!(cond)) {                                                       \
            std::cerr << "CHECK FAILED: " #cond " (" << __FILE__ << ":"      \
                      << __LINE__ << ")\n";                                  \
            std::exit(1);                                                    \
        }                                                                    \
    } while (false)

#define SECTION(name) std::cout << "\n--- " << name << " ---\n"
