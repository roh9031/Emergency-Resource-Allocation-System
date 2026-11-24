/* Leftmost binary search over a vector sorted by a key
 Used by Scenario::findSite (look up a site by id) and as the warm up for
 the A5 binary search on lambda

header only on purpose a class template must be fully visible wherever
 it is instantiated so there is no BinarySearch.cpp (the one deviation
from the one .h/.cpp pair per class rule) */
#pragma once

#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

// T   = element type stored in the vector
// Key = type returned by the key extractor needs operator< and operator==
template <typename T, typename Key>
class BinarySearch {
public:
    using KeyFunction = std::function<Key(const T&)>;

    // keyOf maps an element to the key the vector is sorted by
    // Throws std::invalid_argument if keyOf is empty
    // Time O(1) space O(1)
    explicit BinarySearch(KeyFunction keyOf) : keyOf_(std::move(keyOf)) {
        if (!keyOf_) {
            throw std::invalid_argument("BinarySearch: key function is empty");
        }
    }

    /* Returns the index of the LEFTMOST element whose key equals `key`
       or -1 if there is none
       precondition (not checked checking would cost O(n)): `sorted` is in
       non decreasing order of keyOf_ on unsorted input the result is
       unspecified but the call still terminates
       time O(log n) calls to keyOf_ and key comparisons space O(1) */
    int find(const std::vector<T>& sorted, const Key& key) const {
    int lo = 0;
    int hi = static_cast<int>(sorted.size());

    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;

        Key midKey = keyOf_(sorted[mid]);

        if (midKey < key) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }

    if (lo < static_cast<int>(sorted.size())) {
        if (keyOf_(sorted[lo]) == key) {
            return lo;
        }
    }

    return -1;
}

private:
    KeyFunction keyOf_;
};
