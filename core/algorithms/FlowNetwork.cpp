#include "algorithms/FlowNetwork.h"

#include <stdexcept>

// Time O(V) space O(V)
FlowNetwork::FlowNetwork(int numNodes) {
    if (numNodes <= 0) {
        throw std::invalid_argument("FlowNetwork: numNodes must be positive");
    }
    adj_.resize(numNodes);
}

// Time O(1) amortized space O(1) amortized
int FlowNetwork::addEdge(int from, int to, int capacity, long long cost) {
    checkNode(from);
    checkNode(to);
    if (from == to) {
        throw std::invalid_argument("FlowNetwork::addEdge: self-loop");
    }
    if (capacity < 0) {
        throw std::invalid_argument("FlowNetwork::addEdge: negative capacity");
    }

    /* for my ref:
    
       store the forward edge its paired reverse edge and the
       arc's id then return the id.
      
       
        read each list's size BEFORE pushing onto it that size is the
           index the new edge will land at
             fwdIdx = position the forward edge will get in adj_[from]
             revIdx = position the reverse edge will get in adj_[to]
        Forward edge goes into adj_[from]: Edge{to, revIdx, capacity, cost}
        Reverse edge goes into adj_[to]:   Edge{from, fwdIdx, 0, -cost}
        record where the forward edge lives: edgeRef_.push_back({from, fwdIdx})
        the id is the position of that record in edgeRef_
        sizes are size_t; cast to int with static_cast<int>(...) to avoid
         -Wextra sign warnings. 
    */
      // get the indices before inserting either edge
    int fwdIdx = static_cast<int>(adj_[from].size());
    int revIdx = static_cast<int>(adj_[to].size());

    // insert the forward edge
    adj_[from].push_back({to, revIdx, capacity, cost});

    // insert the reverse residual edge
    adj_[to].push_back({from, fwdIdx, 0, -cost});

    // record the location of the original forward edge
    edgeRef_.push_back({from, fwdIdx});

    // return the original edge's ID.
    return static_cast<int>(edgeRef_.size()) - 1;

    
}

// Time O(1) space O(1)
int FlowNetwork::numNodes() const {
    return static_cast<int>(adj_.size());
}

// Time O(1) space O(1)
int FlowNetwork::numEdges() const {
    return static_cast<int>(edgeRef_.size());
}

// Time O(1) space O(1)
int FlowNetwork::flowOn(int edgeId) const {
    const Edge& e = forwardEdge(edgeId);
    return adj_[e.to][e.rev].cap;
}

// Time O(1) space O(1)
int FlowNetwork::capacityOf(int edgeId) const {
    const Edge& e = forwardEdge(edgeId);
    return e.cap + adj_[e.to][e.rev].cap;
}

// Time O(1) space O(1)
void FlowNetwork::checkNode(int node) const {
    if (node < 0 || node >= numNodes()) {
        throw std::out_of_range("FlowNetwork: node index out of range");
    }
}

// Time O(1) space O(1)
const FlowNetwork::Edge& FlowNetwork::forwardEdge(int edgeId) const {
    if (edgeId < 0 || edgeId >= numEdges()) {
        throw std::out_of_range("FlowNetwork: edge id out of range");
    }
    const auto& ref = edgeRef_[edgeId];
    return adj_[ref.first][ref.second];
}
