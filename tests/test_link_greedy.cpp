// Unit tests for LinkGreedyAllocator (core/algorithms/LinkGreedyAllocator.h)
// Expected allocations worked out by hand from the greedy rule; the max-flow
// totals (A2) come from data/sample/README.md
#include <cstddef>
#include <stdexcept>
#include <string>

#include "algorithms/LinkGreedyAllocator.h"
#include "algorithms/LinkMaxFlowAllocator.h"
#include "io/SampleLoader.h"
#include "test_harness.h"

#ifndef RELIEFROUTE_SOURCE_DIR
#define RELIEFROUTE_SOURCE_DIR "."
#endif

namespace {

void addSite(LoadedData& d, int id, int demand, double urgency) {
    d.sites.emplace_back(id, "Site " + std::to_string(id), demand, urgency, 0.0, 0.0);
}
void addDepot(LoadedData& d, int id, int supply) {
    d.depots.emplace_back(id, "Depot " + std::to_string(id), supply, 0.0, 0.0);
}
void addLink(LoadedData& d, int depotId, int siteId, int capacity) {
    d.links.push_back({depotId, siteId, capacity});
}

long long unitsOn(const AllocationResult& r, int depotId, int siteId) {
    long long total = 0;
    for (const auto& a : r.shipments()) {
        if (a.depotId == depotId && a.siteId == siteId) total += a.units;
    }
    return total;
}

long long unitsTo(const AllocationResult& r, int siteId) {
    long long total = 0;
    for (const auto& a : r.shipments()) {
        if (a.siteId == siteId) total += a.units;
    }
    return total;
}

// Every shipment uses a real link within capacity; depot / site totals stay
// within supply / demand
void checkFeasible(const LoadedData& d, const AllocationResult& r) {
    for (const auto& a : r.shipments()) {
        bool found = false;
        for (const auto& link : d.links) {
            if (link.depotId == a.depotId && link.siteId == a.siteId) {
                found = true;
                CHECK(a.units <= link.capacity);
            }
        }
        CHECK(found);
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
        CHECK(unitsTo(r, site.id()) <= site.demand());
    }
}

LoadedData sample(const std::string& name) {
    return SampleLoader().load(std::string(RELIEFROUTE_SOURCE_DIR) + "/data/sample/" + name);
}

/* 1. Normal case, basic/: X (urgency 10) first takes 10 from A; Y (9) takes the
   2 left at A and 6 from B; Z (4) gets nothing. 18 = all supply, same as A2 */
void testBasicByHand() {
    const LoadedData d = sample("basic");
    const AllocationResult r = LinkGreedyAllocator().allocate(d);
    CHECK_EQ(r.methodId(), std::string("A1"));
    CHECK_EQ(r.totalUnits(), 18LL);
    CHECK_EQ(unitsOn(r, 1, 1), 10LL);  // A -> X
    CHECK_EQ(unitsOn(r, 1, 2), 2LL);   // A -> Y
    CHECK_EQ(unitsOn(r, 2, 2), 6LL);   // B -> Y
    CHECK_EQ(unitsTo(r, 3), 0LL);      // Z (least urgent) is left out
    checkFeasible(d, r);
}

/* 2. Motivating example: greedy ships A -> X first and never undoes it, so Y
   (reachable only from A) gets nothing: 1 kit where max flow (A2) gets 2 */
void testMotivatingGreedyFallsShort() {
    const LoadedData d = sample("motivating");
    const AllocationResult greedy = LinkGreedyAllocator().allocate(d);
    const AllocationResult maxFlow = LinkMaxFlowAllocator().allocate(d);
    CHECK_EQ(greedy.totalUnits(), 1LL);
    CHECK_EQ(unitsOn(greedy, 1, 1), 1LL);  // A -> X
    CHECK_EQ(unitsTo(greedy, 2), 0LL);     // Y stranded
    CHECK_EQ(maxFlow.totalUnits(), 2LL);
    checkFeasible(d, greedy);
}

// 3. Urgency decides who gets scarce supply: one depot, two sites, 5 kits
void testMostUrgentServedFirst() {
    LoadedData d;
    addSite(d, 1, 5, 2.0);  // low urgency, listed first
    addSite(d, 2, 5, 9.0);  // high urgency
    addDepot(d, 1, 5);
    addLink(d, 1, 1, 5);
    addLink(d, 1, 2, 5);
    const AllocationResult r = LinkGreedyAllocator().allocate(d);
    CHECK_EQ(unitsTo(r, 2), 5LL);
    CHECK_EQ(unitsTo(r, 1), 0LL);
}

// 4. Boundary: link capacity, not supply, limits a shipment; capacity 0 ships nothing
void testCapacityLimits() {
    LoadedData d;
    addSite(d, 1, 10, 5.0);
    addDepot(d, 1, 10);
    addDepot(d, 2, 10);
    addLink(d, 1, 1, 3);
    addLink(d, 2, 1, 0);
    const AllocationResult r = LinkGreedyAllocator().allocate(d);
    CHECK_EQ(unitsOn(r, 1, 1), 3LL);
    CHECK_EQ(unitsOn(r, 2, 1), 0LL);
    CHECK_EQ(r.shipments().size(), static_cast<std::size_t>(1));
}

// 5. Empty data and no links: no crash, nothing ships
void testEmptyAndNoLinks() {
    CHECK_EQ(LinkGreedyAllocator().allocate(LoadedData{}).totalUnits(), 0LL);
    LoadedData d;
    addSite(d, 1, 5, 1.0);
    addDepot(d, 1, 5);
    const AllocationResult r = LinkGreedyAllocator().allocate(d);
    CHECK_EQ(r.totalUnits(), 0LL);
    CHECK(r.shipments().empty());
}

// 6. Larger input: large/ (200 sites, 20 depots, 400 links) is feasible and
//    never beats max flow; invalid/ (after the loader) as well
void testLargeAndInvalidSamples() {
    for (const char* name : {"large", "invalid"}) {
        const LoadedData d = sample(name);
        const AllocationResult greedy = LinkGreedyAllocator().allocate(d);
        const AllocationResult maxFlow = LinkMaxFlowAllocator().allocate(d);
        checkFeasible(d, greedy);
        CHECK(greedy.totalUnits() <= maxFlow.totalUnits());
        CHECK(greedy.totalUnits() > 0);
    }
}

// 7. Invalid input is rejected before any work
void testInvalidInputThrows() {
    LoadedData dupDepot;
    addSite(dupDepot, 1, 1, 1.0);
    addDepot(dupDepot, 1, 1);
    addDepot(dupDepot, 1, 2);
    CHECK_THROWS(LinkGreedyAllocator().allocate(dupDepot), std::invalid_argument);

    LoadedData unknownSite;
    addSite(unknownSite, 1, 1, 1.0);
    addDepot(unknownSite, 1, 1);
    addLink(unknownSite, 1, 99, 1);
    CHECK_THROWS(LinkGreedyAllocator().allocate(unknownSite), std::invalid_argument);

    LoadedData negativeCap;
    addSite(negativeCap, 1, 1, 1.0);
    addDepot(negativeCap, 1, 1);
    addLink(negativeCap, 1, 1, -1);
    CHECK_THROWS(LinkGreedyAllocator().allocate(negativeCap), std::invalid_argument);

    LoadedData dupLink;
    addSite(dupLink, 1, 1, 1.0);
    addDepot(dupLink, 1, 1);
    addLink(dupLink, 1, 1, 1);
    addLink(dupLink, 1, 1, 1);
    CHECK_THROWS(LinkGreedyAllocator().allocate(dupLink), std::invalid_argument);
}

// 8. The caller's data is not reordered (the allocator sorts copies)
void testInputUnchanged() {
    const LoadedData d = sample("basic");
    const LoadedData before = d;
    LinkGreedyAllocator().allocate(d);
    CHECK_EQ(d.sites[0].id(), before.sites[0].id());
    CHECK_EQ(d.sites[2].id(), before.sites[2].id());
    CHECK_EQ(d.links[0].siteId, before.links[0].siteId);
}

}  // namespace

void runLinkGreedyTests() {
    RUN_TEST(testBasicByHand);
    RUN_TEST(testMotivatingGreedyFallsShort);
    RUN_TEST(testMostUrgentServedFirst);
    RUN_TEST(testCapacityLimits);
    RUN_TEST(testEmptyAndNoLinks);
    RUN_TEST(testLargeAndInvalidSamples);
    RUN_TEST(testInvalidInputThrows);
    RUN_TEST(testInputUnchanged);
}
