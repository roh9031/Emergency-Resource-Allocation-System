/* A2 for the iteration 3 prototype: maximum flow over the allowed links
 Input is the LoadedData from SampleLoader (sites, depots and links.csv)
 instead of a Scenario in iteration 4 the A2 allocator gets its links from
 travel time <= deadline and implements AllocationStrategy

 Network (node numbers are POSITIONS in the vectors, never real ids):
     node 0                 source
     node 1 + i             depot i      (i = 0..m-1, order of data.depots)
     node 1 + m + j         site j       (j = 0..n-1, order of data.sites)
     node m + n + 1         sink
     source -> depot i      capacity = supply of depot i
     depot  -> site         one arc per link, capacity = link capacity
     site j -> sink         capacity = demand of site j
 Real ids cannot be node numbers: they can be large and sparse (county
 FIPS codes) and depot ids and site ids overlap (depot 1 and site 1) */
#pragma once

#include "domain/Allocation.h"
#include "io/SampleLoader.h"

class LinkMaxFlowAllocator {
public:
    /* Ships as many kits as possible along the allowed links (Edmonds-Karp)
       and returns one shipment per link that carries flow, with REAL ids and
       methodId "A2". Runtime is not set here; the caller times the run
       Throws std::invalid_argument (before any work is done) for:
         a negative depot or site id, a duplicate depot or site id,
         a negative supply, demand or link capacity,
         a link to an unknown depot or site, the same depot -> site link twice
       Empty data gives an empty result
       V = m + n + 2 nodes, E = m + n + L arcs (m depots, n sites, L links)
       Time O(m^2 + n^2 + L^2 + L(m + n)) to validate, plus O(V * E^2) for
       max flow, which dominates; space O(V + E) */
    AllocationResult allocate(const LoadedData& data) const;

private:
    // Throws std::invalid_argument for the cases listed above
    // Time O(m^2 + n^2 + L^2 + L(m + n)) space O(1)
    static void validate(const LoadedData& data);

    /* Position of the depot / site with this real id, or -1 if none
     Linear scan: the input is not sorted and library search is not allowed
    here Once MergeSort exists this can become sort once + BinarySearch */
    // Time O(m) / O(n) space O(1)
    static int indexOfDepot(const LoadedData& data, int id);
    static int indexOfSite(const LoadedData& data, int id);
};
