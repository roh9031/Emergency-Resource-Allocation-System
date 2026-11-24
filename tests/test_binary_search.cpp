// Unit tests for BinarySearch (core/algorithms/BinarySearch.h)
#include <stdexcept>
#include <string>
#include <vector>

#include "algorithms/BinarySearch.h"
#include "test_harness.h"

namespace {

// Identity key so plain int vectors can be searched
BinarySearch<int, int> intSearch() {
    return BinarySearch<int, int>([](const int& x) { return x; });
}

// 1. Invariant: a search without a key function cannot work
void testConstructorRejectsEmptyKeyFunction() {
    BinarySearch<int, int>::KeyFunction none;
    CHECK_THROWS((BinarySearch<int, int>(none)), std::invalid_argument);
}

// 2. Boundary: n == 0 means lo == hi == 0 immediately; the final check
//    must not read sorted[0]
void testEmptyVectorReturnsMinusOne() {
    std::vector<int> a;
    CHECK_EQ(intSearch().find(a, 5), -1);
}

// 3. Smallest non-empty input: hit, key too small, key too large
void testSingleElement() {
    std::vector<int> a = {7};
    BinarySearch<int, int> bs = intSearch();
    CHECK_EQ(bs.find(a, 7), 0);
    CHECK_EQ(bs.find(a, 3), -1);
    CHECK_EQ(bs.find(a, 9), -1);
}

// 4. Left edge: the answer is index 0, so lo must never move past it
void testKeyAtFirstIndex() {
    std::vector<int> a = {1, 3, 5, 7, 9};
    CHECK_EQ(intSearch().find(a, 1), 0);
}

// 5. Right edge: [3, 4] is the counterexample where hi = mid - 1 skips the
//    only match; the bigger array checks the last index in general
void testKeyAtLastIndex() {
    BinarySearch<int, int> bs = intSearch();
    std::vector<int> pair = {3, 4};
    CHECK_EQ(bs.find(pair, 4), 1);
    std::vector<int> a = {1, 3, 5, 7, 9};
    CHECK_EQ(bs.find(a, 9), 4);
}

// 6. Absent keys in all three places: below the minimum, above the maximum
//    (lo ends at n out of bounds) and in a gap (lo ends on a larger key)
void testAbsentKeys() {
    std::vector<int> a = {10, 20, 30, 40};
    BinarySearch<int, int> bs = intSearch();
    CHECK_EQ(bs.find(a, 5), -1);
    CHECK_EQ(bs.find(a, 45), -1);
    CHECK_EQ(bs.find(a, 25), -1);
}

// 7. The contract says LEFTMOST: the first mid lands on a duplicate that
//    is not the leftmost one so an early "== key return mid" fails here
void testDuplicatesReturnLeftmost() {
    BinarySearch<int, int> bs = intSearch();
    std::vector<int> a = {2, 4, 4, 4, 7};
    CHECK_EQ(bs.find(a, 4), 1);
    std::vector<int> same = {5, 5, 5, 5, 5, 5};
    CHECK_EQ(bs.find(same, 5), 0);
}

// 8. Real use: elements are records sorted by an id and the search key
//    has a different type than the element (Scenario::findSite case)
struct Record {
    int id;
    std::string name;
};

void testSearchesRecordsByExtractedKey() {
    std::vector<Record> sites = {
        {3, "Alpha"}, {8, "Bravo"}, {15, "Charlie"}, {21, "Delta"}};
    BinarySearch<Record, int> byId([](const Record& r) { return r.id; });
    int idx = byId.find(sites, 15);
    CHECK_EQ(idx, 2);
    if (idx >= 0) {
        CHECK_EQ(sites[idx].name, std::string("Charlie"));
    }
    CHECK_EQ(byId.find(sites, 4), -1);
}

// 9. Exhaustive small sizes every length 0..33 (odd and even so every
//    way mid can split) and every possible key Expected answers come from
//    how the arrays are built not from another search
void testExhaustiveSmallSizes() {
    BinarySearch<int, int> bs = intSearch();
    for (int n = 0; n <= 33; ++n) {
        // Distinct even values: a[i] = 2i Even key 2i is at i odd is absent
        std::vector<int> evens(n);
        for (int i = 0; i < n; ++i) evens[i] = 2 * i;
        for (int k = -1; k <= 2 * n; ++k) {
            int expected = (k >= 0 && k % 2 == 0 && k / 2 < n) ? k / 2 : -1;
            CHECK_EQ(bs.find(evens, k), expected);
        }

        // Runs of three: a[i] = i / 3 so value v first appears at 3v
        std::vector<int> runs(n);
        for (int i = 0; i < n; ++i) runs[i] = i / 3;
        for (int v = -1; v <= n / 3 + 1; ++v) {
            int expected = (v >= 0 && 3 * v < n) ? 3 * v : -1;
            CHECK_EQ(bs.find(runs, v), expected);
        }
    }
}

}  // namespace

void runBinarySearchTests() {
    RUN_TEST(testConstructorRejectsEmptyKeyFunction);
    RUN_TEST(testEmptyVectorReturnsMinusOne);
    RUN_TEST(testSingleElement);
    RUN_TEST(testKeyAtFirstIndex);
    RUN_TEST(testKeyAtLastIndex);
    RUN_TEST(testAbsentKeys);
    RUN_TEST(testDuplicatesReturnLeftmost);
    RUN_TEST(testSearchesRecordsByExtractedKey);
    RUN_TEST(testExhaustiveSmallSizes);
}
