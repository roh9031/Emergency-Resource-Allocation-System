// Residual flow network shared by all flow-based allocators (A2-A5)
// Every arc added with addEdge() is stored as a forward edge plus a paired
// reverse edge (capacity 0, cost -cost) Each edge records the index of its
// partner so pushing flow updates both directions in O(1)
#pragma once

#include <utility>
#include <vector>

class FlowNetwork {
public:
    // Creates a network with nodes 0..numNodes-1 and no edges
    // Throws std::invalid_argument if numNodes <= 0
    // Time O(V) space O(V)
    explicit FlowNetwork(int numNodes);

    /* Adds arc from -> to with the given capacity and per-unit cost plus its
     reverse residual edge returns the arc's id (0, 1, 2, ... in order)
     throws std::out_of_range if from/to is not in [0, numNodes)
     std::invalid_argument if from == to or capacity < 0
    time O(1) amortized space O(1) amortized 
    */
    int addEdge(int from, int to, int capacity, long long cost = 0);

    // Time O(1) space O(1)
    int numNodes() const;

    // Number of arcs added through addEdge (reverse edges not counted)
    // Time O(1) space O(1)
    int numEdges() const;

    // Units of flow currently on arc edgeId (= its reverse edge's capacity)
    // Throws std::out_of_range for an invalid id
    // Time O(1) space O(1)
    int flowOn(int edgeId) const;

    /* Original capacity of arc edgeId (= forward cap + reverse cap which
     stays constant as flow is pushed) Throws std::out_of_range
    */

    // Time O(1) space O(1)
    int capacityOf(int edgeId) const;

    // Edmonds-Karp: repeatedly pushes flow along a shortest (BFS) augmenting
    // path from s to t until none remains. Returns the flow added by THIS
    // call (a second call on the same network returns 0).
    // Throws std::out_of_range if s/t is not a node, std::invalid_argument
    // if s == t.
    // Time O(V * E^2) (also at most F augmentations, so O(min(F, VE) * E)),
    // space O(V) extra.
    long long maxFlow(int s, int t);

private:
    struct Edge {
        int to;          // head node
        int rev;         // index of the partner edge in adj_[to]
        int cap;         // residual capacity
        long long cost;  // perunit cost (reverse edge holds -cost)
    };

    // Throws std::out_of_range unless 0 <= node < numNodes()
    void checkNode(int node) const;

    // Returns the forward edge for edgeId; throws std::out_of_range
    const Edge& forwardEdge(int edgeId) const;

    /* BFS from s over edges with cap > 0 on return for every reached node
    // v != s: parentNode[v] is the node before v on the path and
    // parentEdge[v] is the index of the edge parentNode[v] -> v inside
    // adj_[parentNode[v]] unreached nodes have parentNode[v] == -1
    // Returns true if t was reached. */
    
    // Time O(V + E) space O(V)
    bool findAugmentingPath(int s, int t,
                            std::vector<int>& parentNode,
                            std::vector<int>& parentEdge) const;

    std::vector<std::vector<Edge>> adj_;       // adj_[u] = edges leaving u
    std::vector<std::pair<int, int>> edgeRef_; // id -> (node, index in adj_[node])
};
