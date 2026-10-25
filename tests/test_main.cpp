// Minimal test runner. Add CHECK(...) calls in new test functions and
// call them from main(). Exit code is non-zero if any check fails, so CI fails.
#include <iostream>
#include "types.h"

static int failures = 0;

#define CHECK(cond)                                                          \
    do {                                                                     \
        if (!(cond)) {                                                       \
            std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ << "  "      \
                      << #cond << "\n";                                      \
            ++failures;                                                      \
        }                                                                    \
    } while (0)

int reliefrouteVersion();

static void testScaffold() {
    CHECK(reliefrouteVersion() == 0);
    Site s;
    CHECK(s.node == -1);
}

int main() {
    testScaffold();
    // testMergeSort();
    // testBinarySearch();
    // testMotivatingExample();
    if (failures == 0) std::cout << "All tests passed.\n";
    return failures == 0 ? 0 : 1;
}
