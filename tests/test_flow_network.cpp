// Unit tests for FlowNetwork (core/algorithms/FlowNetwork.h)
#include <stdexcept>

#include "algorithms/FlowNetwork.h"
#include "test_harness.h"

namespace {

// 1. Invariant a network must have at least one node.
void testConstructorRejectsNonPositiveSize() {
    CHECK_THROWS(FlowNetwork(0), std::invalid_argument);
    CHECK_THROWS(FlowNetwork(-1), std::invalid_argument);
}

// 2. Boundary the smallest valid network
void testSingleNodeNetwork() {
    FlowNetwork net(1);
    CHECK_EQ(net.numNodes(), 1);
    CHECK_EQ(net.numEdges(), 0);
}

// 3. Off by 1 on both ends of the node range for both endpoints
void testAddEdgeRejectsBadNodeIndex() {
    FlowNetwork net(3);
    CHECK_THROWS(net.addEdge(-1, 1, 5), std::out_of_range);
    CHECK_THROWS(net.addEdge(3, 1, 5), std::out_of_range);
    CHECK_THROWS(net.addEdge(0, -1, 5), std::out_of_range);
    CHECK_THROWS(net.addEdge(0, 3, 5), std::out_of_range);
    CHECK_EQ(net.numEdges(), 0);  // failed calls must not add anything
}

// 4. Invariant capacity cannot be negative
void testAddEdgeRejectsNegativeCapacity() {
    FlowNetwork net(2);
    CHECK_THROWS(net.addEdge(0, 1, -1), std::invalid_argument);
    CHECK_EQ(net.numEdges(), 0);
}

// 5. Self loops would make an edge its own reverse partner
void testAddEdgeRejectsSelfLoop() {
    FlowNetwork net(2);
    CHECK_THROWS(net.addEdge(1, 1, 5), std::invalid_argument);
    CHECK_EQ(net.numEdges(), 0);
}

// 6. Allocators store these ids to read flows back later
void testEdgeIdsAreSequential() {
    FlowNetwork net(4);
    CHECK_EQ(net.addEdge(0, 1, 5), 0);
    CHECK_EQ(net.addEdge(1, 2, 3), 1);
    CHECK_EQ(net.addEdge(2, 3, 7, 4), 2);
    CHECK_EQ(net.numEdges(), 3);
}

// 7. A fresh arc carries no flow and reports its full capacity
//    including the zero-capacity case
void testNewEdgeHasZeroFlowAndFullCapacity() {
    FlowNetwork net(3);
    int a = net.addEdge(0, 1, 5, 2);
    int b = net.addEdge(1, 2, 0);
    CHECK_EQ(net.flowOn(a), 0);
    CHECK_EQ(net.capacityOf(a), 5);
    CHECK_EQ(net.flowOn(b), 0);
    CHECK_EQ(net.capacityOf(b), 0);
}

// 8. Parallel arcs and a user added opposite arc are distinct arcs
//    not confused with the internal reverse edge of 0 -> 1.
void testParallelAndOppositeEdgesAreDistinct() {
    FlowNetwork net(2);
    int first = net.addEdge(0, 1, 4);
    int second = net.addEdge(0, 1, 6);
    int back = net.addEdge(1, 0, 9);
    CHECK(first != second);
    CHECK(second != back);
    CHECK_EQ(net.capacityOf(first), 4);
    CHECK_EQ(net.capacityOf(second), 6);
    CHECK_EQ(net.capacityOf(back), 9);
    CHECK_EQ(net.flowOn(back), 0);
}

// 9. Edge id invariant for the read methods
void testReadMethodsRejectBadEdgeId() {
    FlowNetwork net(2);
    net.addEdge(0, 1, 5);
    CHECK_THROWS(net.flowOn(-1), std::out_of_range);
    CHECK_THROWS(net.flowOn(1), std::out_of_range);
    CHECK_THROWS(net.capacityOf(-1), std::out_of_range);
    CHECK_THROWS(net.capacityOf(1), std::out_of_range);
}

}  // namespace

void runFlowNetworkTests() {
    RUN_TEST(testConstructorRejectsNonPositiveSize);
    RUN_TEST(testSingleNodeNetwork);
    RUN_TEST(testAddEdgeRejectsBadNodeIndex);
    RUN_TEST(testAddEdgeRejectsNegativeCapacity);
    RUN_TEST(testAddEdgeRejectsSelfLoop);
    RUN_TEST(testEdgeIdsAreSequential);
    RUN_TEST(testNewEdgeHasZeroFlowAndFullCapacity);
    RUN_TEST(testParallelAndOppositeEdgesAreDistinct);
    RUN_TEST(testReadMethodsRejectBadEdgeId);
}
