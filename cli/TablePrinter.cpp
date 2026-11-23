#include "TablePrinter.h"

#include <iomanip>
#include <sstream>
#include <unordered_map>

namespace {

// "Depot A (1)" or just "#7" if the id is not in the loaded data
template <typename T>
std::string label(const std::unordered_map<int, const T*>& byId, int id) {
    const auto it = byId.find(id);
    if (it == byId.end()) {
        return "#" + std::to_string(id);
    }
    return it->second->name() + " (" + std::to_string(id) + ")";
}

std::string percent(long long part, long long whole) {
    if (whole == 0) {
        return "n/a";  // a site with zero demand has no meaningful fill rate
    }
    std::ostringstream s;
    s << std::fixed << std::setprecision(1) << (100.0 * part / whole) << "%";
    return s.str();
}

std::string rule(std::size_t width) { return std::string(width, '-'); }

}  // namespace

/* Pseudocode
   index depots and sites by id
   for each shipment: print depot label, site label, units; add units to delivered[site]
   print totals: supply = sum of depot supply, demand = sum of site demand,
                 delivered = result.totalUnits(), overall fill = delivered / demand
   for each site in file order: print demand, delivered[site], unmet, fill rate
   Time O(K + S + D) expected, space O(S + D) */
void TablePrinter::printAllocation(const AllocationResult& result, const LoadedData& data,
                                   std::ostream& out) {
    std::unordered_map<int, const Depot*> depotById;
    std::unordered_map<int, const Site*> siteById;
    for (const auto& d : data.depots) depotById[d.id()] = &d;
    for (const auto& s : data.sites) siteById[s.id()] = &s;

    // Column widths fit the longest label so the table stays aligned
    std::size_t depotW = 5, siteW = 4;  // "Depot", "Site"
    for (const auto& a : result.shipments()) {
        depotW = std::max(depotW, label(depotById, a.depotId).size());
        siteW = std::max(siteW, label(siteById, a.siteId).size());
    }
    const std::size_t unitsW = 7;

    out << "\nAllocation (" << result.methodId() << ")\n";
    out << std::left << std::setw(static_cast<int>(depotW)) << "Depot" << "  ->  "
        << std::setw(static_cast<int>(siteW)) << "Site" << "  " << std::right
        << std::setw(static_cast<int>(unitsW)) << "Units" << "\n";
    out << rule(depotW + 6 + siteW + 2 + unitsW) << "\n";

    std::unordered_map<int, long long> delivered;
    for (const auto& a : result.shipments()) {
        out << std::left << std::setw(static_cast<int>(depotW)) << label(depotById, a.depotId)
            << "  ->  " << std::setw(static_cast<int>(siteW)) << label(siteById, a.siteId)
            << "  " << std::right << std::setw(static_cast<int>(unitsW)) << a.units << "\n";
        delivered[a.siteId] += a.units;
    }
    if (result.shipments().empty()) {
        out << "(no shipments)\n";
    }

    long long totalSupply = 0, totalDemand = 0;
    for (const auto& d : data.depots) totalSupply += d.supply();
    for (const auto& s : data.sites) totalDemand += s.demand();

    out << "\nSummary\n";
    out << "  Total supply : " << totalSupply << "\n";
    out << "  Total demand : " << totalDemand << "\n";
    out << "  Delivered    : " << result.totalUnits() << "  ("
        << percent(result.totalUnits(), totalDemand) << " of demand)\n";

    std::size_t nameW = 4;
    for (const auto& s : data.sites) nameW = std::max(nameW, label(siteById, s.id()).size());
    out << "\n" << std::left << std::setw(static_cast<int>(nameW)) << "Site" << std::right
        << std::setw(9) << "Demand" << std::setw(11) << "Delivered" << std::setw(8) << "Unmet"
        << std::setw(9) << "Fill" << "\n";
    out << rule(nameW + 37) << "\n";
    for (const auto& s : data.sites) {
        const long long got = delivered.count(s.id()) ? delivered[s.id()] : 0;
        out << std::left << std::setw(static_cast<int>(nameW)) << label(siteById, s.id())
            << std::right << std::setw(9) << s.demand() << std::setw(11) << got
            << std::setw(8) << (s.demand() - got) << std::setw(9) << percent(got, s.demand())
            << "\n";
    }
    if (result.runtimeMs() > 0.0) {
        out << "\nRuntime: " << std::fixed << std::setprecision(3) << result.runtimeMs()
            << " ms\n";
        out.unsetf(std::ios::fixed);
    }
}

// Time O(P) space O(1)
void TablePrinter::printProblems(const std::vector<std::string>& problems, std::ostream& out) {
    if (problems.empty()) {
        out << "No problems: every line loaded.\n";
        return;
    }
    out << problems.size() << " line(s) skipped:\n";
    for (const auto& p : problems) {
        out << "  " << p << "\n";
    }
}

// Time O(1) space O(1)
void TablePrinter::printLoadSummary(const std::string& folder, const LoadedData& data,
                                    std::ostream& out) {
    out << "Loaded " << folder << ": " << data.sites.size() << " sites, "
        << data.depots.size() << " depots, " << data.links.size() << " links";
    if (!data.problems.empty()) {
        out << ", " << data.problems.size() << " bad line(s) skipped (menu option 2)";
    }
    out << "\n";
}
