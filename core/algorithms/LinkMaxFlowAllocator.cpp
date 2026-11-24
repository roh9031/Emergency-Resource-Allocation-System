#include "algorithms/LinkMaxFlowAllocator.h"

#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

#include "algorithms/FlowNetwork.h"

/* Pseudocode
   validate(data)
   m = #depots, n = #sites, L = #links
   S = 0, T = m + n + 1, net = FlowNetwork(m + n + 2)
   for each depot i:  net.addEdge(S, 1 + i, supply_i)
   for each site j:   net.addEdge(1 + m + j, T, demand_j)
   for each link k:   d = indexOfDepot(depotId), s = indexOfSite(siteId)
                      linkEdge[k] = net.addEdge(1 + d, 1 + m + s, capacity)
   net.maxFlow(S, T)
   for each link k:   result.add(depotId, siteId, net.flowOn(linkEdge[k]))
   Time O(V * E^2) dominated by maxFlow space O(V + E) */
AllocationResult LinkMaxFlowAllocator::allocate(const LoadedData& data) const {
    validate(data);

    const int m = static_cast<int>(data.depots.size());
    const int n = static_cast<int>(data.sites.size());
    const int source = 0;
    const int sink = m + n + 1;
    FlowNetwork net(m + n + 2);  // at least 2 nodes so empty data is fine

    // linkEdge[k] = arc id returned by addEdge for data.links[k], so the flow
    // on link k can be read back in O(1) after maxFlow
    std::vector<int> linkEdge(data.links.size(), -1);

    // Build the network

    // 1. Source -> depot i
    // for my ref: source connects to every depot limiting flow by its supply
    for (int i = 0; i < m; ++i) {
        int depotNode = 1 + i;
        net.addEdge(source, depotNode, data.depots[i].supply());
    }

    // 2. Site j -> sink
    // for my ref: every site connects to sink limiting flow by its demand
    for (int j = 0; j < n; ++j) {
        int siteNode = 1 + m + j;
        net.addEdge(siteNode, sink, data.sites[j].demand());
    }

    // 3. Depot -> site links
    // for my ref: only allowed connections from links.csv get an edge
    // node numbers use vector positions not the real depot/site IDs
    for (std::size_t k = 0; k < data.links.size(); ++k) {
        int d = indexOfDepot(data, data.links[k].depotId);
        int s = indexOfSite(data, data.links[k].siteId);

        int depotNode = 1 + d;
        int siteNode = 1 + m + s;

        // for my ref: save the edge ID so we can retrieve its flow later
        linkEdge[k] = net.addEdge(
            depotNode,
            siteNode,
            data.links[k].capacity
        );
    }

    net.maxFlow(source, sink);

    AllocationResult result("A2");

    // extract the flow into shipments
    // for my ref: edge ids let us read each links flow directly
    // use the real IDs here not the network's internal node numbers
    for (std::size_t k = 0; k < data.links.size(); ++k) {
        result.add(
            data.links[k].depotId,
            data.links[k].siteId,
            net.flowOn(linkEdge[k])
        );
    }

    return result;
}

// Time O(m^2 + n^2 + L^2 + L(m + n)) space O(1)
void LinkMaxFlowAllocator::validate(const LoadedData& data) {
    const auto& depots = data.depots;
    const auto& sites = data.sites;
    const auto& links = data.links;

    for (std::size_t i = 0; i < depots.size(); ++i) {
        const int id = depots[i].id();
        if (id < 0) {
            throw std::invalid_argument("LinkMaxFlowAllocator: negative depot id " +
                                        std::to_string(id));
        }
        // Safety net: Depots constructor already rejects this
        if (depots[i].supply() < 0) {
            throw std::invalid_argument("LinkMaxFlowAllocator: negative supply at depot " +
                                        std::to_string(id));
        }
        for (std::size_t k = 0; k < i; ++k) {
            if (depots[k].id() == id) {
                throw std::invalid_argument("LinkMaxFlowAllocator: duplicate depot id " +
                                            std::to_string(id));
            }
        }
    }

    for (std::size_t j = 0; j < sites.size(); ++j) {
        const int id = sites[j].id();
        if (id < 0) {
            throw std::invalid_argument("LinkMaxFlowAllocator: negative site id " +
                                        std::to_string(id));
        }
        // Safety net: Site's constructor already rejects this
        if (sites[j].demand() < 0) {
            throw std::invalid_argument("LinkMaxFlowAllocator: negative demand at site " +
                                        std::to_string(id));
        }
        for (std::size_t k = 0; k < j; ++k) {
            if (sites[k].id() == id) {
                throw std::invalid_argument("LinkMaxFlowAllocator: duplicate site id " +
                                            std::to_string(id));
            }
        }
    }       

    /* A link with a negative id is reported as unknown: no depot or site can
     have a negative id after the checks above */
    for (std::size_t k = 0; k < links.size(); ++k) {
        const Link& link = links[k];
        const std::string where = " (link " + std::to_string(link.depotId) + " -> " +
                                  std::to_string(link.siteId) + ")";
        if (link.capacity < 0) {
            throw std::invalid_argument("LinkMaxFlowAllocator: negative capacity" + where);
        }
        if (indexOfDepot(data, link.depotId) == -1) {
            throw std::invalid_argument("LinkMaxFlowAllocator: unknown depot id" + where);
        }
        if (indexOfSite(data, link.siteId) == -1) {
            throw std::invalid_argument("LinkMaxFlowAllocator: unknown site id" + where);
        }
        for (std::size_t p = 0; p < k; ++p) {
            if (links[p].depotId == link.depotId && links[p].siteId == link.siteId) {
                throw std::invalid_argument("LinkMaxFlowAllocator: duplicate link" + where);
            }
        }
    }
}

// Time O(m) space O(1)
int LinkMaxFlowAllocator::indexOfDepot(const LoadedData& data, int id) {
    for (std::size_t i = 0; i < data.depots.size(); ++i) {
        if (data.depots[i].id() == id) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// Time O(n) space O(1)
int LinkMaxFlowAllocator::indexOfSite(const LoadedData& data, int id) {
    for (std::size_t j = 0; j < data.sites.size(); ++j) {
        if (data.sites[j].id() == id) {
            return static_cast<int>(j);
        }
    }
    return -1;
}
