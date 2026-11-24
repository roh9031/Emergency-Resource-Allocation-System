#include "algorithms/LinkGreedyAllocator.h"

#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

#include "algorithms/BinarySearch.h"
#include "algorithms/MergeSort.h"
#include "algorithms/UrgencyOrder.h"

namespace {

std::string linkText(const Link& link) {
    return " (link " + std::to_string(link.depotId) + " -> " + std::to_string(link.siteId) +
           ")";
}

int smallest(int a, int b, int c) {
    int s = a < b ? a : b;
    return s < c ? s : c;
}

}  // namespace

/* Pseudocode
   order      = sites sorted by UrgencyOrder            (MergeSort, stable)
   depotsById = depots sorted by id; left[d] = supply   (MergeSort)
   bySite     = links sorted by siteId                  (MergeSort, stable:
                                                         file order kept per site)
   validate: negative / duplicate ids (adjacent after sorting), negative capacity,
             unknown depot or site (BinarySearch miss), duplicate link
   for each site s in order:
       need = demand(s)
       k = leftmost link in bySite with siteId == s.id  (BinarySearch)
       while k < L and bySite[k].siteId == s.id and need > 0:
           d = index of bySite[k].depotId in depotsById  (BinarySearch)
           amount = min(left[d], need, capacity of link k)
           if amount > 0: ship amount on link k; left[d] -= amount; need -= amount
           k = k + 1
   Time: three sorts O(n log n + m log m + L log L), then each link is looked
   at once with one O(log m) depot search: O(L log m). Space O(n + m + L) */
AllocationResult LinkGreedyAllocator::allocate(const LoadedData& data) const {
    // --- 1. Sort copies of the input (the caller's data is never modified)
    std::vector<Site> order = data.sites;
    MergeSort<Site>(urgencyOrder).sort(order);

    std::vector<Depot> depotsById = data.depots;
    MergeSort<Depot>([](const Depot& a, const Depot& b) { return a.id() < b.id(); })
        .sort(depotsById);

    std::vector<Link> bySite = data.links;
    MergeSort<Link>([](const Link& a, const Link& b) { return a.siteId < b.siteId; })
        .sort(bySite);

    const BinarySearch<Depot, int> findDepot([](const Depot& d) { return d.id(); });
    const BinarySearch<Link, int> findFirstLink([](const Link& l) { return l.siteId; });

    // --- 2. Validate (all checks before any shipment is made)
    for (std::size_t i = 0; i < depotsById.size(); ++i) {
        const int id = depotsById[i].id();
        if (id < 0) {
            throw std::invalid_argument("LinkGreedyAllocator: negative depot id " +
                                        std::to_string(id));
        }
        if (i > 0 && depotsById[i - 1].id() == id) {  // sorted: duplicates are adjacent
            throw std::invalid_argument("LinkGreedyAllocator: duplicate depot id " +
                                        std::to_string(id));
        }
    }
    std::vector<Site> sitesById = data.sites;
    MergeSort<Site>([](const Site& a, const Site& b) { return a.id() < b.id(); })
        .sort(sitesById);
    for (std::size_t j = 0; j < sitesById.size(); ++j) {
        const int id = sitesById[j].id();
        if (id < 0) {
            throw std::invalid_argument("LinkGreedyAllocator: negative site id " +
                                        std::to_string(id));
        }
        if (j > 0 && sitesById[j - 1].id() == id) {
            throw std::invalid_argument("LinkGreedyAllocator: duplicate site id " +
                                        std::to_string(id));
        }
    }
    const BinarySearch<Site, int> findSite([](const Site& s) { return s.id(); });

    std::vector<Link> byPair = data.links;  // (siteId, depotId) order: duplicates adjacent
    MergeSort<Link>([](const Link& a, const Link& b) {
        return a.siteId != b.siteId ? a.siteId < b.siteId : a.depotId < b.depotId;
    }).sort(byPair);
    for (std::size_t k = 0; k < byPair.size(); ++k) {
        const Link& link = byPair[k];
        if (link.capacity < 0) {
            throw std::invalid_argument("LinkGreedyAllocator: negative capacity" +
                                        linkText(link));
        }
        if (findDepot.find(depotsById, link.depotId) == -1) {
            throw std::invalid_argument("LinkGreedyAllocator: unknown depot id" +
                                        linkText(link));
        }
        if (findSite.find(sitesById, link.siteId) == -1) {
            throw std::invalid_argument("LinkGreedyAllocator: unknown site id" +
                                        linkText(link));
        }
        if (k > 0 && byPair[k - 1].siteId == link.siteId &&
            byPair[k - 1].depotId == link.depotId) {
            throw std::invalid_argument("LinkGreedyAllocator: duplicate link" +
                                        linkText(link));
        }
    }

    // --- 3. Greedy: most urgent site first, never undo a shipment
    std::vector<int> left(depotsById.size());  // remaining supply, same order as depotsById
    for (std::size_t i = 0; i < depotsById.size(); ++i) {
        left[i] = depotsById[i].supply();
    }

    AllocationResult result("A1");
    for (const Site& site : order) {
        int need = site.demand();
        int k = findFirstLink.find(bySite, site.id());
        if (k == -1) {
            continue;  // no allowed links: this site gets nothing
        }
        while (k < static_cast<int>(bySite.size()) && bySite[k].siteId == site.id() &&
               need > 0) {
            const Link& link = bySite[k];
            const int d = findDepot.find(depotsById, link.depotId);
            const int amount = smallest(left[d], need, link.capacity);
            if (amount > 0) {
                result.add(link.depotId, link.siteId, amount);
                left[d] -= amount;
                need -= amount;
            }
            ++k;
        }
    }
    return result;
}
