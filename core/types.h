// Shared data types for ReliefRoute.
// Intial scaffold
#pragma once

#include <string>
#include <vector>

struct Site {
    int id = 0;
    std::string name;
    int demand = 0;        // units needed (d_i)
    double urgency = 0.0;  // weight w_i > 0
    double lat = 0.0, lon = 0.0;
    int node = -1;         // nearest road-graph node, -1 if not snapped
};

struct Depot {
    int id = 0;
    std::string name;
    int supply = 0;        // stock s_j
    double lat = 0.0, lon = 0.0;
    int node = -1;
};

// Edge in the residual flow network. `rev` is the index of the reverse edge
// in adj[to], so an augmentation updates both directions in O(1).
struct Edge {
    int to = 0;
    int rev = 0;
    int cap = 0;
    long long cost = 0;
};

struct Allocation {
    int depotId = 0;
    int siteId = 0;
    int units = 0;
};

struct Params {
    double deadline = 0.0;     // tau: connection eligible if travel time <= tau
    int linkCapacity = 0;      // u_ji, 0 = unlimited
    double alpha = 0.0;        // travel-time cost factor (0 in main experiments)
    unsigned int seed = 0;
};

struct Metrics {
    long long delivered = 0;
    double weightedValue = 0.0;
    double gap = 0.0;               // (W* - W) / W*
    double criticalCoverage = 0.0;  // share of critical sites with fill >= 0.5
    int sitesWithNothing = 0;
    double jain = 0.0;
    double gini = 0.0;
    double runtimeMs = 0.0;
};

struct Scenario {
    std::string id;
    std::vector<Site> sites;              // kept sorted by id for binary search
    std::vector<Depot> depots;
    std::vector<std::vector<double>> travel;  // [depot][site] minutes
    Params params;
};
