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

// maxFlow

// MF1. Input invariants endpoints must be nodes and distinct
void testMaxFlowRejectsBadEndpoints() {
    FlowNetwork net(3);
    net.addEdge(0, 2, 5);
    CHECK_THROWS(net.maxFlow(-1, 2), std::out_of_range);
    CHECK_THROWS(net.maxFlow(0, 3), std::out_of_range);
    CHECK_THROWS(net.maxFlow(1, 1), std::invalid_argument);
}

// MF2. Smallest non trivial network one arc
void testMaxFlowSingleEdge() {
    FlowNetwork net(2);
    int e = net.addEdge(0, 1, 5);
    CHECK_EQ(net.maxFlow(0, 1), 5LL);
    CHECK_EQ(net.flowOn(e), 5);
}

// MF3. t unreachable BFS must stop and report no path
void testMaxFlowUnreachableSink() {
    FlowNetwork net(4);
    int a = net.addEdge(0, 1, 5);
    int b = net.addEdge(2, 3, 5);
    CHECK_EQ(net.maxFlow(0, 3), 0LL);
    CHECK_EQ(net.flowOn(a), 0);
    CHECK_EQ(net.flowOn(b), 0);
}

// MF4. BFS must skip edges with zero residual capacity
void testMaxFlowZeroCapacityBlocksPath() {
    FlowNetwork net(3);
    net.addEdge(0, 1, 0);
    net.addEdge(1, 2, 5);
    CHECK_EQ(net.maxFlow(0, 2), 0LL);
}

// MF5. Bottleneck = smallest capacity along the path
void testMaxFlowSeriesBottleneck() {
    FlowNetwork net(3);
    int a = net.addEdge(0, 1, 10);
    int b = net.addEdge(1, 2, 3);
    CHECK_EQ(net.maxFlow(0, 2), 3LL);
    CHECK_EQ(net.flowOn(a), 3);
    CHECK_EQ(net.flowOn(b), 3);
}

// MF6. Several augmentations must add up
void testMaxFlowTwoDisjointPaths() {
    FlowNetwork net(4);
    net.addEdge(0, 1, 4);
    net.addEdge(1, 3, 4);
    net.addEdge(0, 2, 6);
    net.addEdge(2, 3, 6);
    CHECK_EQ(net.maxFlow(0, 3), 10LL);
}

/* MF7. in our proposal's motivating example arc order makes the first BFS take
s->A->X->t so reaching flow 2 REQUIRES the reverse edge X->A
the maximum flow is unique (Y is reachable only from A) so asserting
exact per arc flows is safe here */
void testMaxFlowNeedsReverseEdge() {
    const int s = 0, A = 1, B = 2, X = 3, Y = 4, t = 5;
    FlowNetwork net(6);
    net.addEdge(s, A, 1);
    net.addEdge(s, B, 1);
    int ax = net.addEdge(A, X, 1);
    int ay = net.addEdge(A, Y, 1);
    int bx = net.addEdge(B, X, 1);
    net.addEdge(X, t, 1);
    net.addEdge(Y, t, 1);
    CHECK_EQ(net.maxFlow(s, t), 2LL);
    CHECK_EQ(net.flowOn(ax), 0);
    CHECK_EQ(net.flowOn(ay), 1);
    CHECK_EQ(net.flowOn(bx), 1);
}

// MF8. textbook network (CLRS Figure "flow network") max flow 23
// has a cycle (v1 -> v3 -> v2 -> v1) and cross arcs
void testMaxFlowClrsExample() {
    const int s = 0, v1 = 1, v2 = 2, v3 = 3, v4 = 4, t = 5;
    FlowNetwork net(6);
    net.addEdge(s, v1, 16);
    net.addEdge(s, v2, 13);
    net.addEdge(v1, v3, 12);
    net.addEdge(v2, v1, 4);
    net.addEdge(v3, v2, 9);
    net.addEdge(v2, v4, 14);
    net.addEdge(v4, v3, 7);
    net.addEdge(v3, t, 20);
    net.addEdge(v4, t, 4);
    CHECK_EQ(net.maxFlow(s, t), 23LL);
}

// MF9. data/sample scenario with full reachability (step 1) many maximum
// splits exist so check the total and the constraints never one split
void testMaxFlowSampleScenarioConstraints() {
    const int s = 0, t = 6;
    const int depots[2] = {1, 2};
    const int supply[2] = {12, 6};
    const int sites[3] = {3, 4, 5};
    const int demand[3] = {10, 8, 5};

    FlowNetwork net(7);
    int sourceArc[2];
    int sinkArc[3];
    int link[2][3];
    for (int j = 0; j < 2; ++j) {
        sourceArc[j] = net.addEdge(s, depots[j], supply[j]);
    }
    for (int j = 0; j < 2; ++j) {
        for (int i = 0; i < 3; ++i) {
            int cap = supply[j] < demand[i] ? supply[j] : demand[i];
            link[j][i] = net.addEdge(depots[j], sites[i], cap);
        }
    }
    for (int i = 0; i < 3; ++i) {
        sinkArc[i] = net.addEdge(sites[i], t, demand[i]);
    }

    CHECK_EQ(net.maxFlow(s, t), 18LL);

    // Capacity 0 <= flow <= capacity on every arc
    for (int id = 0; id < net.numEdges(); ++id) {
        CHECK(net.flowOn(id) >= 0);
        CHECK(net.flowOn(id) <= net.capacityOf(id));
    }
    // Depots conservation (in == out) and supply limit
    for (int j = 0; j < 2; ++j) {
        int out = 0;
        for (int i = 0; i < 3; ++i) out += net.flowOn(link[j][i]);
        CHECK_EQ(out, net.flowOn(sourceArc[j]));
        CHECK(out <= supply[j]);
    }
    // Sites conservation and demand limit
    int delivered = 0;
    for (int i = 0; i < 3; ++i) {
        int in = net.flowOn(link[0][i]) + net.flowOn(link[1][i]);
        CHECK_EQ(in, net.flowOn(sinkArc[i]));
        CHECK(in <= demand[i]);
        delivered += in;
    }
    CHECK_EQ(delivered, 18);
}

// MF10. Contract maxFlow returns the flow added by this call
void testMaxFlowSecondCallAddsNothing() {
    FlowNetwork net(3);
    int a = net.addEdge(0, 1, 4);
    int b = net.addEdge(1, 2, 4);
    CHECK_EQ(net.maxFlow(0, 2), 4LL);
    CHECK_EQ(net.maxFlow(0, 2), 0LL);
    CHECK_EQ(net.flowOn(a), 4);
    CHECK_EQ(net.flowOn(b), 4);
}

// MF11. Bottleneck in the middle of a longer path: the push must walk the
// WHOLE path, so every arc carries the flow, not just the first or last
void testMaxFlowLongPathMiddleBottleneck() {
    FlowNetwork net(5);
    int ids[4];
    ids[0] = net.addEdge(0, 1, 9);
    ids[1] = net.addEdge(1, 2, 7);
    ids[2] = net.addEdge(2, 3, 2);
    ids[3] = net.addEdge(3, 4, 8);
    CHECK_EQ(net.maxFlow(0, 4), 2LL);
    for (int i = 0; i < 4; ++i) {
        CHECK_EQ(net.flowOn(ids[i]), 2);
    }
}

// MF12. Invariant forward cap + reverse cap == original capacity on every
// arc after many augmentations (fails if the push forgets the reverse edge
// or updates the wrong partner)
void testMaxFlowKeepsCapacitiesConstant() {
    const int s = 0, v1 = 1, v2 = 2, v3 = 3, v4 = 4, t = 5;
    const int from[9] = {s, s, v1, v2, v3, v2, v4, v3, v4};
    const int to[9] = {v1, v2, v3, v1, v2, v4, v3, t, t};
    const int cap[9] = {16, 13, 12, 4, 9, 14, 7, 20, 4};
    FlowNetwork net(6);
    for (int i = 0; i < 9; ++i) {
        net.addEdge(from[i], to[i], cap[i]);
    }
    CHECK_EQ(net.maxFlow(s, t), 23LL);
    for (int id = 0; id < 9; ++id) {
        CHECK_EQ(net.capacityOf(id), cap[id]);
    }
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

    RUN_TEST(testMaxFlowRejectsBadEndpoints);
    RUN_TEST(testMaxFlowSingleEdge);
    RUN_TEST(testMaxFlowUnreachableSink);
    RUN_TEST(testMaxFlowZeroCapacityBlocksPath);
    RUN_TEST(testMaxFlowSeriesBottleneck);
    RUN_TEST(testMaxFlowTwoDisjointPaths);
    RUN_TEST(testMaxFlowNeedsReverseEdge);
    RUN_TEST(testMaxFlowClrsExample);
    RUN_TEST(testMaxFlowSampleScenarioConstraints);
    RUN_TEST(testMaxFlowSecondCallAddsNothing);
    RUN_TEST(testMaxFlowLongPathMiddleBottleneck);
    RUN_TEST(testMaxFlowKeepsCapacitiesConstant);
}
