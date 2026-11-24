#include "algorithms/FlowNetwork.h"

#include <stdexcept>
#include <queue>
#include<limits>

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

// Edmonds-Karp maximum flow
// Time O(V * E^2) space O(V) extra
long long FlowNetwork::maxFlow(int s, int t) {
    checkNode(s);
    checkNode(t);
    if (s == t) {
        throw std::invalid_argument("FlowNetwork::maxFlow: s == t");
    }

    std::vector<int> parentNode(numNodes(), -1);
    std::vector<int> parentEdge(numNodes(), -1);
    long long total = 0;

    while (findAugmentingPath(s, t, parentNode, parentEdge)) {
        /*push the bottleneck along the path BFS just found
        
           Pass 1 find the bottleneck b
             start b at std::numeric_limits<int>::max() (needs <limits>)
             walk v from t back to s  u = parentNode[v],
             the edge used is adj_[u][parentEdge[v]]; b = min(b, its cap)
        
           Pass 2 push b units (same walk)
             take the edge BY REFERENCE (Edge& e = ) otherwise 
             modify a copy and nothing changes -> infinite loop
             e.cap -= b;   its partner is adj_[v][e.rev] add b to it
        
           Finally total += b
        
            */

                total += pushAlongPath(s, t, parentNode, parentEdge);
    }
    return total;
}   
        
// Push the bottleneck along a stored s -> t path
// Time O(path length) = O(V), space O(1)
int FlowNetwork::pushAlongPath(int s, int t,
                              const std::vector<int>& parentNode,
                              const std::vector<int>& parentEdge) {
    if (parentNode[t] == -1) {
        throw std::logic_error(
            "FlowNetwork::pushAlongPath: no path stored"
        );
    }

    // Pass 1 find the bottleneck capacity
    int bottleneck = std::numeric_limits<int>::max();

    for (int v = t; v != s; v = parentNode[v]) {
        int u = parentNode[v];
        int edgeIdx = parentEdge[v];

        // for my ref adj_[u][edgeIdx] is the edge used to reach v
        const Edge& e = adj_[u][edgeIdx];

        if (e.cap < bottleneck) {
            bottleneck = e.cap;
        }
    }

    // Pass 2 push flow along the path
    for (int v = t; v != s; v = parentNode[v]) {
        int u = parentNode[v];
        int edgeIdx = parentEdge[v];

        // for my ref e.rev gives the index of the reverse edge in adj_[v]
        Edge& e = adj_[u][edgeIdx];

        e.cap -= bottleneck;
        adj_[v][e.rev].cap += bottleneck;
    }

    return bottleneck;
}

// BFS for a shortest augmenting path
// Time O(V + E) space O(V)
bool FlowNetwork::findAugmentingPath(int s, int t,
                                     std::vector<int>& parentNode,
                                     std::vector<int>& parentEdge) const {
    /* breadth first search from s using only edges with cap > 0.
    
       for my ref
        1. Reset parentNode to all -1 (it is reused across iterations) then
           set parentNode[s] = s so s counts as visited
        2. Use a std::queue<int> ; push s
        3. While the queue is not empty: pop u; loop over i = 0 
           adj_[u].size()-1 with e = adj_[u][i]. If e.cap > 0 and
           parentNode[e.to] == -1: record parentNode[e.to] = u and
           parentEdge[e.to] = i; if e.to == t return true; else push e.to
        4. Queue emptied without reaching t -> return false
        5. Use an index loop (not range-for) so we have i for parentEdge 
    */

    for (int i = 0; i < numNodes(); ++i) {
    parentNode[i] = -1;
    parentEdge[i] = -1;
}

    // Mark source as visited
    parentNode[s] = s;

    std::queue<int> q;
    q.push(s);

    while (!q.empty()) {

        int u = q.front();
        q.pop();

        // Examine every outgoing residual edge from u
        for (int i = 0; i < static_cast<int>(adj_[u].size()); ++i) {

            const Edge& e = adj_[u][i];

            // Only consider edges with positive residual capacity
            // Also skip nodes that have already been visited
            if (e.cap > 0 && parentNode[e.to] == -1) {

                // Record how we reached this node.
                parentNode[e.to] = u;
                parentEdge[e.to] = i;

                // Destination reached augmenting path found
                if (e.to == t) {
                    return true;
                }

                q.push(e.to);
            }
        }
    }

    // No augmenting path exists.
    return false;
  
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
