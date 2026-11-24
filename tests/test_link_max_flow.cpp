// Unit tests for LinkMaxFlowAllocator (core/algorithms/LinkMaxFlowAllocator.h)
// Expected totals are the oracles from data/sample/README.md
#include <cstddef>
#include <stdexcept>
#include <string>

#include "algorithms/LinkMaxFlowAllocator.h"
#include "io/SampleLoader.h"
#include "test_harness.h"

#ifndef RELIEFROUTE_SOURCE_DIR
#define RELIEFROUTE_SOURCE_DIR "."
#endif

namespace {

// Coordinates and urgency do not matter to max flow fixed placeholders
void addSite(LoadedData& d, int id, int demand) {
    d.sites.emplace_back(id, "Site " + std::to_string(id), demand, 1.0, 0.0, 0.0);
}
void addDepot(LoadedData& d, int id, int supply) {
    d.depots.emplace_back(id, "Depot " + std::to_string(id), supply, 0.0, 0.0);
}
void addLink(LoadedData& d, int depotId, int siteId, int capacity) {
    d.links.push_back({depotId, siteId, capacity});
}

// Units shipped depotId -> siteId in r (0 if no shipment) summed over entries
long long unitsOn(const AllocationResult& r, int depotId, int siteId) {
    long long total = 0;
    for (const auto& a : r.shipments()) {
        if (a.depotId == depotId && a.siteId == siteId) {
            total += a.units;
        }
    }
    return total;
}

// data/sample/basic built by hand - depots 1 (12), 2 (6); sites 1 X (10),
// 2 Y (8), 3 Z (5); all 6 links with capacity min(supply, demand)
LoadedData basic() {
    LoadedData d;
    addSite(d, 1, 10);
    addSite(d, 2, 8);
    addSite(d, 3, 5);
    addDepot(d, 1, 12);
    addDepot(d, 2, 6);
    addLink(d, 1, 1, 10);
    addLink(d, 1, 2, 8);
    addLink(d, 1, 3, 5);
    addLink(d, 2, 1, 6);
    addLink(d, 2, 2, 6);
    addLink(d, 2, 3, 5);
    return d;
}

// data/sample/motivating built by hand: A=1, B=2 (supply 1); X=1, Y=2
// (demand 1); links A->X, A->Y, B->X capacity 1
LoadedData motivating() {
    LoadedData d;
    addSite(d, 1, 1);
    addSite(d, 2, 1);
    addDepot(d, 1, 1);
    addDepot(d, 2, 1);
    addLink(d, 1, 1, 1);
    addLink(d, 1, 2, 1);
    addLink(d, 2, 1, 1);
    return d;
}

/* Every shipment uses a real link, fits its capacity and per depot / per
site totals stay within supply / demand shared by several tests */
void checkFeasible(const LoadedData& d, const AllocationResult& r) {
    for (const auto& a : r.shipments()) {
        bool found = false;
        for (const auto& link : d.links) {
            if (link.depotId == a.depotId && link.siteId == a.siteId) {
                found = true;
                CHECK(a.units <= link.capacity);
            }
        }
        CHECK(found);  // fails if an index was printed instead of a real id
        CHECK(a.units > 0);
    }
    for (const auto& depot : d.depots) {
        long long out = 0;
        for (const auto& a : r.shipments()) {
            if (a.depotId == depot.id()) out += a.units;
        }
        CHECK(out <= depot.supply());
    }
    for (const auto& site : d.sites) {
        long long in = 0;
        for (const auto& a : r.shipments()) {
            if (a.siteId == site.id()) in += a.units;
        }
        CHECK(in <= site.demand());
    }
}

/* 1. Normal case: basic/ ships its whole supply 18. Only the total and
 feasibility are checked many maximum splits exist */
void testBasicShipsEighteen() {
    const LoadedData d = basic();
    const AllocationResult r = LinkMaxFlowAllocator().allocate(d);
    CHECK_EQ(r.totalUnits(), 18LL);
    CHECK_EQ(r.methodId(), std::string("A2"));
    checkFeasible(d, r);
}

/* 2. Motivating example: the only maximum allocation is A->Y, B->X a greedy
   A->X first gets 1 reaching 2 needs the reverse edge to undo it */
void testMotivatingExactAllocation() {
    const LoadedData d = motivating();
    const AllocationResult r = LinkMaxFlowAllocator().allocate(d);
    CHECK_EQ(r.totalUnits(), 2LL);
    CHECK_EQ(r.shipments().size(), static_cast<std::size_t>(2));
    CHECK_EQ(unitsOn(r, 1, 2), 1LL);  // A -> Y
    CHECK_EQ(unitsOn(r, 2, 1), 1LL);  // B -> X
    CHECK_EQ(unitsOn(r, 1, 1), 0LL);  // A -> X was undone
}

// 3. Empty data: no crash an empty A2 result (the network has just S and T)
void testEmptyData() {
    const AllocationResult r = LinkMaxFlowAllocator().allocate(LoadedData{});
    CHECK_EQ(r.shipments().size(), static_cast<std::size_t>(0));
    CHECK_EQ(r.totalUnits(), 0LL);
    CHECK_EQ(r.methodId(), std::string("A2"));
}

// 4. Supply and demand but no links: no path nothing ships
void testNoLinksShipsNothing() {
    LoadedData d;
    addSite(d, 1, 5);
    addDepot(d, 1, 5);
    const AllocationResult r = LinkMaxFlowAllocator().allocate(d);
    CHECK_EQ(r.totalUnits(), 0LL);
    CHECK_EQ(r.shipments().size(), static_cast<std::size_t>(0));
}

// 5. Link capacity really reaches the network: supply 10, demand 10, link 3
//    In basic/ capacities never bind so a bug that ignores them would pass 1
void testLinkCapacityBinds() {
    LoadedData d;
    addSite(d, 1, 10);
    addDepot(d, 1, 10);
    addLink(d, 1, 1, 3);
    const AllocationResult r = LinkMaxFlowAllocator().allocate(d);
    CHECK_EQ(r.totalUnits(), 3LL);
    CHECK_EQ(unitsOn(r, 1, 1), 3LL);
}

/* 6. Non-contiguous real ids (FIPS-like sites, depots 7 and 3 out of order)
   catch mixing up positions and ids. Supply 9 + 5 = 14 < demand 17 and
   14 is reachable (7->12086 6, 7->12033 3, 3->12011 4, 3->12033 1) */
void testRealIdsAndConstraints() {
    LoadedData d;
    addSite(d, 12086, 6);
    addSite(d, 12011, 4);
    addSite(d, 12033, 7);
    addDepot(d, 7, 9);
    addDepot(d, 3, 5);
    addLink(d, 7, 12086, 6);
    addLink(d, 7, 12011, 2);
    addLink(d, 3, 12011, 4);
    addLink(d, 3, 12033, 5);
    addLink(d, 7, 12033, 3);
    const AllocationResult r = LinkMaxFlowAllocator().allocate(d);
    CHECK_EQ(r.totalUnits(), 14LL);
    checkFeasible(d, r);
}

/* 7. A link to an id that is not loaded: depot and site checked separately,
  each lookup can fail on its own */
void testUnknownIdsThrow() {
    LoadedData d = basic();
    addLink(d, 99, 1, 1);
    CHECK_THROWS(LinkMaxFlowAllocator().allocate(d), std::invalid_argument);

    LoadedData e = basic();
    addLink(e, 1, 99, 1);
    CHECK_THROWS(LinkMaxFlowAllocator().allocate(e), std::invalid_argument);
}

/* 8. Negative link capacity (Negative supply/demand cannot be built: the
    Site and Depot constructors throw see test_site.cpp / test_depot.cpp) */
void testNegativeCapacityThrows() {
    LoadedData d = basic();
    d.links[0].capacity = -1;
    CHECK_THROWS(LinkMaxFlowAllocator().allocate(d), std::invalid_argument);
}

// 9. Duplicate ids make positions ambiguous: which depot is "id 2"?
void testDuplicateIdsThrow() {
    LoadedData d = basic();
    addDepot(d, 2, 4);
    CHECK_THROWS(LinkMaxFlowAllocator().allocate(d), std::invalid_argument);

    LoadedData e = basic();
    addSite(e, 3, 4);
    CHECK_THROWS(LinkMaxFlowAllocator().allocate(e), std::invalid_argument);
}

/* 10. Decision 1: the same depot -> site link twice is rejected so the
   result has at most one shipment per pair */
void testDuplicateLinkThrows() {
    LoadedData d = basic();
    addLink(d, 1, 1, 2);
    CHECK_THROWS(LinkMaxFlowAllocator().allocate(d), std::invalid_argument);
}

// 11. Decision 2: negative ids are rejected up front before max flow runs
void testNegativeIdsThrow() {
    LoadedData d;
    addSite(d, 1, 5);
    addDepot(d, -1, 5);
    CHECK_THROWS(LinkMaxFlowAllocator().allocate(d), std::invalid_argument);

    LoadedData e;
    addSite(e, -4, 5);
    addDepot(e, 1, 5);
    CHECK_THROWS(LinkMaxFlowAllocator().allocate(e), std::invalid_argument);
}

std::string sample(const std::string& name) {
    return std::string(RELIEFROUTE_SOURCE_DIR) + "/data/sample/" + name;
}

// 12. End to end with the real loader: invalid/ after the 7 bad lines are
//     skipped gives 17 different from basic/'s 18 on purpose
void testInvalidSampleThroughLoader() {
    const LoadedData d = SampleLoader().load(sample("invalid"));
    const AllocationResult r = LinkMaxFlowAllocator().allocate(d);
    CHECK_EQ(r.totalUnits(), 17LL);
    checkFeasible(d, r);
}

// 13. Scale: large/ (200 sites, 20 depots, 400 links) gives total supply
//     3990 proven in scripts/make_sample_large.py
void testLargeSampleThroughLoader() {
    const LoadedData d = SampleLoader().load(sample("large"));
    const AllocationResult r = LinkMaxFlowAllocator().allocate(d);
    CHECK_EQ(r.totalUnits(), 3990LL);
    checkFeasible(d, r);
}

}  // namespace

void runLinkMaxFlowTests() {
    RUN_TEST(testBasicShipsEighteen);
    RUN_TEST(testMotivatingExactAllocation);
    RUN_TEST(testEmptyData);
    RUN_TEST(testNoLinksShipsNothing);
    RUN_TEST(testLinkCapacityBinds);
    RUN_TEST(testRealIdsAndConstraints);
    RUN_TEST(testUnknownIdsThrow);
    RUN_TEST(testNegativeCapacityThrows);
    RUN_TEST(testDuplicateIdsThrow);
    RUN_TEST(testDuplicateLinkThrows);
    RUN_TEST(testNegativeIdsThrow);
    RUN_TEST(testInvalidSampleThroughLoader);
    RUN_TEST(testLargeSampleThroughLoader);
}
