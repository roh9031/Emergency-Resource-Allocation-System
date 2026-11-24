// Unit tests for Params (core/domain/Params.h)
#include <limits>
#include <stdexcept>

#include "domain/Params.h"
#include "test_harness.h"

namespace {

// 1. Normal input: every value read back unchanged
void testConstructorStoresValues() {
    Params p(120.0, 25, 0.5, 42u);
    CHECK_EQ(p.deadline(), 120.0);
    CHECK_EQ(p.linkCapacity(), 25);
    CHECK_EQ(p.alpha(), 0.5);
    CHECK_EQ(p.seed(), 42u);
}

// 2. Boundary: all zeros are valid (deadline 0, unlimited links, alpha 0),
//    and an infinite deadline means "no limit"
void testBoundaryValuesAllowed() {
    Params p(0.0, 0, 0.0, 0u);
    CHECK_EQ(p.deadline(), 0.0);
    CHECK_EQ(p.linkCapacity(), 0);
    CHECK_EQ(p.alpha(), 0.0);
    const double inf = std::numeric_limits<double>::infinity();
    Params q(inf, 0, 0.0, 1u);
    CHECK_EQ(q.deadline(), inf);
}

// 3. Invalid: each negative field is rejected on its own
void testNegativeValuesRejected() {
    CHECK_THROWS(Params(-1.0, 0, 0.0, 0u), std::invalid_argument);
    CHECK_THROWS(Params(10.0, -1, 0.0, 0u), std::invalid_argument);
    CHECK_THROWS(Params(10.0, 0, -0.1, 0u), std::invalid_argument);
}

// 4. Invalid: NaN deadline or alpha
void testNaNRejected() {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    CHECK_THROWS(Params(nan, 0, 0.0, 0u), std::invalid_argument);
    CHECK_THROWS(Params(10.0, 0, nan, 0u), std::invalid_argument);
}

}  // namespace

void runParamsTests() {
    RUN_TEST(testConstructorStoresValues);
    RUN_TEST(testBoundaryValuesAllowed);
    RUN_TEST(testNegativeValuesRejected);
    RUN_TEST(testNaNRejected);
}
