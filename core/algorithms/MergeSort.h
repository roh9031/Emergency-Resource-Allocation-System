/* Stable merge sort over a vector, ordered by a caller-supplied "less"
 Used by the greedy allocator (sites by urgency, depots and links by id) and,
 in iteration 4, by Scenario (sites by id) and sitesByUrgency()

 Stable: elements that compare equal keep their original relative order.
 The greedy allocator relies on this so links for the same site stay in
 links.csv order.

 Header-only on purpose: a class template must be fully visible wherever it
 is instantiated (same reason as BinarySearch.h) */
#pragma once

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

template <typename T>
class MergeSort {
public:
    using Less = std::function<bool(const T&, const T&)>;

    // less(a, b) is true when a must come before b (a strict weak ordering)
    // Throws std::invalid_argument if less is empty
    // Time O(1) space O(1)
    explicit MergeSort(Less less) : less_(std::move(less)) {
        if (!less_) {
            throw std::invalid_argument("MergeSort: comparison function is empty");
        }
    }

    /* Sorts items in place, stable
       Pseudocode (top-down)
         sort(a, lo, hi):                  // sorts a[lo, hi)
             if hi - lo <= 1: return
             mid = lo + (hi - lo) / 2
             sort(a, lo, mid); sort(a, mid, hi)
             merge(a, lo, mid, hi)
         merge: copy a[lo, hi) to buffer; i = lo, j = mid
             repeatedly take buffer[j] only if less(buffer[j], buffer[i]),
             otherwise buffer[i]   <- taking the left one on ties = stable
       Time O(n log n) comparisons in every case (best, average, worst)
       Space O(n) for the buffer + O(log n) recursion depth */
    void sort(std::vector<T>& items) const {
        if (items.size() < 2) {
            return;
        }
        std::vector<T> buffer(items);  // copies, so T needs no default constructor
        sortRange(items, buffer, 0, items.size());
    }

private:
    // Sorts items[lo, hi) using buffer as scratch space of the same size
    void sortRange(std::vector<T>& items, std::vector<T>& buffer, std::size_t lo,
                   std::size_t hi) const {
        if (hi - lo <= 1) {
            return;
        }
        const std::size_t mid = lo + (hi - lo) / 2;
        sortRange(items, buffer, lo, mid);
        sortRange(items, buffer, mid, hi);
        // Already in order: skip the merge (keeps sorted input at O(n))
        if (!less_(items[mid], items[mid - 1])) {
            return;
        }
        merge(items, buffer, lo, mid, hi);
    }

    // Merges the sorted runs items[lo, mid) and items[mid, hi)
    // Time O(hi - lo) space O(1) beyond buffer
    void merge(std::vector<T>& items, std::vector<T>& buffer, std::size_t lo,
               std::size_t mid, std::size_t hi) const {
        for (std::size_t k = lo; k < hi; ++k) {
            buffer[k] = items[k];
        }
        std::size_t i = lo;
        std::size_t j = mid;
        for (std::size_t k = lo; k < hi; ++k) {
            if (i >= mid) {
                items[k] = buffer[j++];
            } else if (j >= hi) {
                items[k] = buffer[i++];
            } else if (less_(buffer[j], buffer[i])) {  // strictly less: right wins
                items[k] = buffer[j++];
            } else {                                    // tie or left smaller: left wins
                items[k] = buffer[i++];
            }
        }
    }

    Less less_;
};
