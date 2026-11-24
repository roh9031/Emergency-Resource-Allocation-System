// Unit tests for MergeSort (core/algorithms/MergeSort.h) and UrgencyOrder
#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "algorithms/MergeSort.h"
#include "algorithms/UrgencyOrder.h"
#include "domain/Site.h"
#include "test_harness.h"

namespace {

bool intLess(const int& a, const int& b) { return a < b; }

// True if v is in non-decreasing order (checked by hand, no library sort)
bool isSorted(const std::vector<int>& v) {
    for (std::size_t i = 1; i < v.size(); ++i) {
        if (v[i] < v[i - 1]) return false;
    }
    return true;
}

// 1. Normal input: an unsorted list comes out sorted
void testSortsIntegers() {
    std::vector<int> v{5, 2, 9, 1, 5, 6, 0, -3};
    MergeSort<int>(intLess).sort(v);
    CHECK(v == (std::vector<int>{-3, 0, 1, 2, 5, 5, 6, 9}));
}

// 2. Boundary: empty and single-element vectors are left as they are
void testEmptyAndSingle() {
    std::vector<int> empty;
    MergeSort<int>(intLess).sort(empty);
    CHECK(empty.empty());
    std::vector<int> one{42};
    MergeSort<int>(intLess).sort(one);
    CHECK_EQ(one.size(), static_cast<std::size_t>(1));
    CHECK_EQ(one[0], 42);
}

// 3. Already sorted and reverse sorted (best and worst input order)
void testSortedAndReversed() {
    std::vector<int> up{1, 2, 3, 4, 5, 6, 7};
    MergeSort<int>(intLess).sort(up);
    CHECK(up == (std::vector<int>{1, 2, 3, 4, 5, 6, 7}));
    std::vector<int> down{7, 6, 5, 4, 3, 2, 1};
    MergeSort<int>(intLess).sort(down);
    CHECK(down == (std::vector<int>{1, 2, 3, 4, 5, 6, 7}));
}

// 4. Stability: equal keys keep their original order (the greedy relies on it)
void testStable() {
    // (key, original position)
    std::vector<std::pair<int, int>> v{{2, 0}, {1, 1}, {2, 2}, {1, 3}, {2, 4}, {1, 5}};
    MergeSort<std::pair<int, int>>(
        [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
            return a.first < b.first;  // compares the key only
        })
        .sort(v);
    const std::vector<std::pair<int, int>> expected{{1, 1}, {1, 3}, {1, 5},
                                                    {2, 0}, {2, 2}, {2, 4}};
    CHECK(v == expected);
}

// 5. Larger input: 2000 values in a scrambled order, then all equal
void testLargeInput() {
    std::vector<int> v;
    for (int i = 0; i < 2000; ++i) {
        v.push_back((i * 7919) % 2003);  // a fixed permutation-like scramble
    }
    MergeSort<int>(intLess).sort(v);
    CHECK_EQ(v.size(), static_cast<std::size_t>(2000));
    CHECK(isSorted(v));
    std::vector<int> same(500, 3);
    MergeSort<int>(intLess).sort(same);
    CHECK(same == std::vector<int>(500, 3));
}

// 6. Invalid: an empty comparison function is rejected
void testEmptyComparatorThrows() {
    CHECK_THROWS(MergeSort<int>(MergeSort<int>::Less{}), std::invalid_argument);
}

// 7. UrgencyOrder: urgency desc, then demand desc, then id asc
void testUrgencyOrder() {
    std::vector<Site> sites{
        Site(5, "E", 10, 2.0, 0, 0),
        Site(3, "C", 4, 9.0, 0, 0),
        Site(1, "A", 7, 9.0, 0, 0),  // same urgency as C, more demand -> first
        Site(4, "D", 7, 9.0, 0, 0),  // ties with A on both -> smaller id (A) first
        Site(2, "B", 1, 5.5, 0, 0),
    };
    MergeSort<Site>(urgencyOrder).sort(sites);
    std::vector<int> ids;
    for (const auto& s : sites) ids.push_back(s.id());
    CHECK(ids == (std::vector<int>{1, 4, 3, 2, 5}));
}

}  // namespace

void runMergeSortTests() {
    RUN_TEST(testSortsIntegers);
    RUN_TEST(testEmptyAndSingle);
    RUN_TEST(testSortedAndReversed);
    RUN_TEST(testStable);
    RUN_TEST(testLargeInput);
    RUN_TEST(testEmptyComparatorThrows);
    RUN_TEST(testUrgencyOrder);
}
