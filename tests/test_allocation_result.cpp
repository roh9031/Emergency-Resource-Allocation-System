// Unit tests for AllocationResult (core/domain/Allocation.h)
#include <limits>
#include <stdexcept>
#include <string>

#include "domain/Allocation.h"
#include "test_harness.h"

namespace {

// 1. Invariant: every result says which method produced it
void testConstructorRejectsEmptyMethodId() {
    CHECK_THROWS(AllocationResult(""), std::invalid_argument);
}

// 2. A fresh result has a well-defined starting state
void testNewResultIsEmpty() {
    AllocationResult r("A2");
    CHECK_EQ(r.methodId(), std::string("A2"));
    CHECK_EQ(r.shipments().size(), static_cast<std::size_t>(0));
    CHECK_EQ(r.totalUnits(), 0LL);
    CHECK_EQ(r.runtimeMs(), 0.0);
}

// 3. Writers and the CLI print shipments in insertion order with real ids
void testAddStoresShipmentsInOrder() {
    AllocationResult r("A1");
    r.add(2, 12086, 5);
    r.add(1, 12011, 3);
    CHECK_EQ(r.shipments().size(), static_cast<std::size_t>(2));
    if (r.shipments().size() == 2) {
        CHECK_EQ(r.shipments()[0].depotId, 2);
        CHECK_EQ(r.shipments()[0].siteId, 12086);
        CHECK_EQ(r.shipments()[0].units, 5);
        CHECK_EQ(r.shipments()[1].depotId, 1);
        CHECK_EQ(r.shipments()[1].siteId, 12011);
        CHECK_EQ(r.shipments()[1].units, 3);
    }
    CHECK_EQ(r.totalUnits(), 8LL);
}

// 4. Allocators may call add for every link; zero is not a shipment
void testAddIgnoresZeroUnits() {
    AllocationResult r("A2");
    r.add(1, 5, 0);
    CHECK_EQ(r.shipments().size(), static_cast<std::size_t>(0));
    CHECK_EQ(r.totalUnits(), 0LL);
}

// 5. Validate before changing anything: a failed add leaves no trace
void testAddRejectsNegativesAndLeavesStateUnchanged() {
    AllocationResult r("A4");
    r.add(1, 5, 4);
    CHECK_THROWS(r.add(1, 5, -1), std::invalid_argument);
    CHECK_THROWS(r.add(-1, 5, 2), std::invalid_argument);
    CHECK_THROWS(r.add(1, -1, 2), std::invalid_argument);
    CHECK_EQ(r.shipments().size(), static_cast<std::size_t>(1));
    CHECK_EQ(r.totalUnits(), 4LL);
}

// 6. The running total must not overflow int (3e9 > INT_MAX ~ 2.1e9)
void testTotalUnitsDoesNotOverflowInt() {
    AllocationResult r("A2");
    r.add(1, 1, 1000000000);
    r.add(1, 2, 1000000000);
    r.add(1, 3, 1000000000);
    CHECK_EQ(r.totalUnits(), 3000000000LL);
}

// 7. Documented decision: the same pair twice stays two entries
//    (AllocationValidator reports duplicates, not this class)
void testDuplicatePairKeptAsTwoEntries() {
    AllocationResult r("A1");
    r.add(1, 7, 2);
    r.add(1, 7, 3);
    CHECK_EQ(r.shipments().size(), static_cast<std::size_t>(2));
    CHECK_EQ(r.totalUnits(), 5LL);
}

// 8. Runtime invariant, including the NaN trap (NaN < 0 is false) and
//    infinity (not a measured duration)
void testSetRuntimeMs() {
    AllocationResult r("A5");
    r.setRuntimeMs(12.5);
    CHECK_EQ(r.runtimeMs(), 12.5);
    r.setRuntimeMs(0.0);
    CHECK_EQ(r.runtimeMs(), 0.0);
    CHECK_THROWS(r.setRuntimeMs(-0.1), std::invalid_argument);
    CHECK_THROWS(r.setRuntimeMs(std::numeric_limits<double>::quiet_NaN()),
                 std::invalid_argument);
    CHECK_THROWS(r.setRuntimeMs(std::numeric_limits<double>::infinity()),
                 std::invalid_argument);
    CHECK_EQ(r.runtimeMs(), 0.0);  // failed calls keep the old value
}

}  // namespace

void runAllocationResultTests() {
    RUN_TEST(testConstructorRejectsEmptyMethodId);
    RUN_TEST(testNewResultIsEmpty);
    RUN_TEST(testAddStoresShipmentsInOrder);
    RUN_TEST(testAddIgnoresZeroUnits);
    RUN_TEST(testAddRejectsNegativesAndLeavesStateUnchanged);
    RUN_TEST(testTotalUnitsDoesNotOverflowInt);
    RUN_TEST(testDuplicatePairKeptAsTwoEntries);
    RUN_TEST(testSetRuntimeMs);
}
