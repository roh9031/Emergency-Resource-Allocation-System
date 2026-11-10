/*
Minimal assert style test helper for ReliefRoute (no external framework)
 Each test file defines one run<Class>Tests() function declared below and
 called from main() in test_main.cpp a failed check is counted and reported
 but does not stop the run so every failure shows up in a single run 
 */
#pragma once

#include <exception>
#include <iostream>

namespace testing {

// Number of failed checks in this run C++17 inline variable one shared
// instance across all test translation units.
inline int failures = 0;

}  // namespace testing

// Time O(1) space O(1) beyond evaluating `cond`
#define CHECK(cond)                                                        \
    do {                                                                   \
        if (!(cond)) {                                                     \
            ++testing::failures;                                           \
            std::cerr << __FILE__ << ":" << __LINE__                       \
                      << ": CHECK(" #cond ") failed\n";                    \
        }                                                                  \
    } while (0)

/* Time O(1) space O(1) beyond evaluating the arguments both sides are
 evaluated exactly once Requires operator== and operator<< for the type. */
#define CHECK_EQ(actual, expected)                                         \
    do {                                                                   \
        const auto checkActual_ = (actual);                                \
        const auto checkExpected_ = (expected);                            \
        if (!(checkActual_ == checkExpected_)) {                           \
            ++testing::failures;                                           \
            std::cerr << __FILE__ << ":" << __LINE__                       \
                      << ": CHECK_EQ(" #actual ", " #expected ") got "     \
                      << checkActual_ << ", expected " << checkExpected_   \
                      << "\n";                                             \
        }                                                                  \
    } while (0)

// Passes only if `expr` throws exactly ExceptionType (or a subclass of it)
// Time O(1) space O(1) beyond evaluating `expr`
#define CHECK_THROWS(expr, ExceptionType)                                  \
    do {                                                                   \
        try {                                                              \
            (void)(expr);                                                  \
            ++testing::failures;                                           \
            std::cerr << __FILE__ << ":" << __LINE__                       \
                      << ": CHECK_THROWS(" #expr ") did not throw\n";      \
        } catch (const ExceptionType&) {                                   \
        } catch (...) {                                                    \
            ++testing::failures;                                           \
            std::cerr << __FILE__ << ":" << __LINE__                       \
                      << ": CHECK_THROWS(" #expr ") threw the wrong type, "\
                         "expected " #ExceptionType "\n";                  \
        }                                                                  \
    } while (0)

// Runs one test function. An unexpected exception counts as one failure
// instead of aborting the whole run, so the remaining tests still execute.
// Time O(1) overhead beyond the test itself.
#define RUN_TEST(testFn)                                                   \
    do {                                                                   \
        try {                                                              \
            testFn();                                                      \
        } catch (const std::exception& e) {                                \
            ++testing::failures;                                           \
            std::cerr << #testFn ": unexpected exception: " << e.what()    \
                      << "\n";                                             \
        } catch (...) {                                                    \
            ++testing::failures;                                           \
            std::cerr << #testFn ": unexpected non-std exception\n";       \
        }                                                                  \
    } while (0)

// One entry point per class under test add a declaration here and a call
// in test_main.cpp for every new test file
void runFlowNetworkTests();
