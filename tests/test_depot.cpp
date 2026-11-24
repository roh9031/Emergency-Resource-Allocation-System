// Unit tests for Depot (core/domain/Depot.h)
#include <stdexcept>
#include <string>

#include "domain/Depot.h"
#include "test_harness.h"

namespace {

// 1. Normal input: every value read back unchanged, node starts unsnapped
void testConstructorStoresValues() {
    Depot d(2, "Depot B", 6, 35.52, -82.65);
    CHECK_EQ(d.id(), 2);
    CHECK_EQ(d.name(), std::string("Depot B"));
    CHECK_EQ(d.supply(), 6);
    CHECK_EQ(d.lat(), 35.52);
    CHECK_EQ(d.lon(), -82.65);
    CHECK_EQ(d.node(), -1);
}

// 2. Boundary: an empty depot (supply 0) is allowed
void testZeroSupplyAllowed() {
    Depot d(1, "Empty", 0, 0.0, 0.0);
    CHECK_EQ(d.supply(), 0);
}

// 3. Invalid: negative supply (same error as invalid/depots.csv line 3)
void testNegativeSupplyRejected() {
    CHECK_THROWS(Depot(2, "Depot B", -5, 35.52, -82.65), std::invalid_argument);
}

// 4. setNode snaps the depot; anything below -1 is a bad index
void testSetNode() {
    Depot d(1, "A", 10, 0.0, 0.0);
    d.setNode(7);
    CHECK_EQ(d.node(), 7);
    CHECK_THROWS(d.setNode(-5), std::out_of_range);
    CHECK_EQ(d.node(), 7);
}

}  // namespace

void runDepotTests() {
    RUN_TEST(testConstructorStoresValues);
    RUN_TEST(testZeroSupplyAllowed);
    RUN_TEST(testNegativeSupplyRejected);
    RUN_TEST(testSetNode);
}
