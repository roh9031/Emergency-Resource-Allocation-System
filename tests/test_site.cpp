// Unit tests for Site (core/domain/Site.h)
#include <limits>
#include <stdexcept>
#include <string>

#include "domain/Site.h"
#include "test_harness.h"

namespace {

// 1. Normal input: every value read back unchanged, node starts unsnapped
void testConstructorStoresValues() {
    Site s(12086, "Miami-Dade", 40, 0.87, 25.76, -80.19);
    CHECK_EQ(s.id(), 12086);
    CHECK_EQ(s.name(), std::string("Miami-Dade"));
    CHECK_EQ(s.demand(), 40);
    CHECK_EQ(s.urgency(), 0.87);
    CHECK_EQ(s.lat(), 25.76);
    CHECK_EQ(s.lon(), -80.19);
    CHECK_EQ(s.node(), -1);
}

// 2. Boundary: zero demand is allowed (a site that needs nothing)
void testZeroDemandAllowed() {
    Site s(1, "X", 0, 1.0, 0.0, 0.0);
    CHECK_EQ(s.demand(), 0);
}

// 3. Invalid: negative demand
void testNegativeDemandRejected() {
    CHECK_THROWS(Site(1, "X", -1, 1.0, 0.0, 0.0), std::invalid_argument);
}

// 4. Invalid: urgency must be strictly positive (0, negative, NaN)
void testNonPositiveUrgencyRejected() {
    CHECK_THROWS(Site(1, "X", 5, 0.0, 0.0, 0.0), std::invalid_argument);
    CHECK_THROWS(Site(1, "X", 5, -2.0, 0.0, 0.0), std::invalid_argument);
    CHECK_THROWS(Site(1, "X", 5, std::numeric_limits<double>::quiet_NaN(), 0.0, 0.0),
                 std::invalid_argument);
}

// 5. setNode snaps the site; -1 resets it; anything below -1 is a bad index
void testSetNode() {
    Site s(1, "X", 5, 1.0, 0.0, 0.0);
    s.setNode(42);
    CHECK_EQ(s.node(), 42);
    s.setNode(-1);
    CHECK_EQ(s.node(), -1);
    CHECK_THROWS(s.setNode(-2), std::out_of_range);
    CHECK_EQ(s.node(), -1);  // unchanged after the failed call
}

}  // namespace

void runSiteTests() {
    RUN_TEST(testConstructorStoresValues);
    RUN_TEST(testZeroDemandAllowed);
    RUN_TEST(testNegativeDemandRejected);
    RUN_TEST(testNonPositiveUrgencyRejected);
    RUN_TEST(testSetNode);
}
