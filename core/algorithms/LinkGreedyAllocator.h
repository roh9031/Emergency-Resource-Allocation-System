/* A1 for the iteration 3 prototype: urgency-first greedy over the allowed links
 Input is the LoadedData from SampleLoader, like LinkMaxFlowAllocator (A2).
 In iteration 4 GreedyAllocator gets its links from travel time <= deadline
 and implements AllocationStrategy

 Idea: serve the most urgent site first, as fully as its links allow (its
 depots tried in ascending id order, as core/README.md specifies when there
 are no travel times), then the next most urgent, and so on. A shipment is
 never taken back. The result does not depend on the row order of links.csv
 This is fast and simple, but it can strand supply: on the motivating
 example it ships A->X first, after which Y (reachable only from A) gets
 nothing, so A1 delivers 1 kit where A2 (max flow) delivers 2 */
#pragma once

#include "domain/Allocation.h"
#include "io/SampleLoader.h"

class LinkGreedyAllocator {
public:
    /* Returns one shipment per (depot, site) pair that carries kits, with REAL
       ids and methodId "A1". Runtime is not set here; the caller times the run
       Throws std::invalid_argument (before any work is done) for:
         a negative or duplicate depot or site id, a negative link capacity,
         a link to an unknown depot or site, the same depot -> site link twice
       Empty data gives an empty result
       m depots, n sites, L links
       Time O(n log n + m log m + L log L + L log m), space O(n + m + L) */
    AllocationResult allocate(const LoadedData& data) const;
};
