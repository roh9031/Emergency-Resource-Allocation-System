/* The order in which the greedy allocator (A1) serves sites:
   1. higher urgency first
   2. then higher demand first
   3. then smaller id first   (makes the order total, so results are
                               reproducible even with equal urgency/demand)
 Used with MergeSort<Site>. Header-only: one small inline function */
#pragma once

#include "domain/Site.h"

// True when site a must be served before site b
// Time O(1) space O(1)
inline bool urgencyOrder(const Site& a, const Site& b) {
    if (a.urgency() != b.urgency()) {
        return a.urgency() > b.urgency();
    }
    if (a.demand() != b.demand()) {
        return a.demand() > b.demand();
    }
    return a.id() < b.id();
}
