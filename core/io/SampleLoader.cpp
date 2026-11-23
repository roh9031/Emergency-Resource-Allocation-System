#include "io/SampleLoader.h"

#include <cerrno>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include <utility>

#include "io/CsvReader.h"

namespace {

// Whole-string integer parse; false for "", "abc", "3.5", "12x", overflow
// Time O(c) space O(1)
bool parseInt(const std::string& s, int& out) {
    if (s.empty()) {
        return false;
    }
    errno = 0;
    char* end = nullptr;
    const long long v = std::strtoll(s.c_str(), &end, 10);
    if (*end != '\0' || errno == ERANGE || v < INT_MIN || v > INT_MAX) {
        return false;
    }
    out = static_cast<int>(v);
    return true;
}

// Whole-string finite double parse; false for "", "abc", "1.2.3", nan, inf
// Time O(c) space O(1)
bool parseDouble(const std::string& s, double& out) {
    if (s.empty()) {
        return false;
    }
    errno = 0;
    char* end = nullptr;
    const double v = std::strtod(s.c_str(), &end);
    if (*end != '\0' || errno == ERANGE || !std::isfinite(v)) {
        return false;
    }
    out = v;
    return true;
}

// Collects "<file>:<line>: <message>" strings
class Reporter {
public:
    Reporter(std::string fileName, std::vector<std::string>& sink)
        : fileName_(std::move(fileName)), sink_(sink) {}
    void add(int line, const std::string& message) {
        sink_.push_back(fileName_ + ":" + std::to_string(line) + ": " + message);
    }

private:
    std::string fileName_;
    std::vector<std::string>& sink_;
};

std::string wrongFieldCount(std::size_t expected, std::size_t got) {
    return "wrong number of fields (expected " + std::to_string(expected) +
           ", got " + std::to_string(got) + ")";
}

// Reads one file: CsvReader problems are reported, line 1 (header) dropped
// Time O(C) space O(C)
std::vector<CsvReader::Row> readDataRows(const std::string& folder,
                                         const std::string& fileName,
                                         Reporter& report) {
    CsvReader reader(folder + "/" + fileName);
    std::vector<CsvReader::Row> rows = reader.read();  // throws if missing
    for (const auto& p : reader.problems()) {
        report.add(p.lineNumber, p.message);
    }
    std::vector<CsvReader::Row> data;
    for (auto& row : rows) {
        if (row.lineNumber != 1) {
            data.push_back(std::move(row));
        }
    }
    return data;
}

}  // namespace

/* Pseudocode
   for each data line of sites.csv:
       if field count != 6 or a number field does not parse: report, skip
       if demand < 0 or urgency <= 0: report, skip
       if id already seen: report "duplicate site id (first seen on line L)", skip
       append Site, remember id -> line
   same for depots.csv (5 fields, supply >= 0)
   for each data line of links.csv:
       if field count != 3 or a field does not parse: report, skip
       if capacity < 0: report, skip
       if depotId or siteId was not loaded: report, skip
       if (depotId, siteId) already linked: report, skip
       append Link
   Time O(C + S + D + K) expected (hash lookups O(1) expected,
   duplicate-link set O(K log K)), space O(C) */
LoadedData SampleLoader::load(const std::string& folder) const {
    LoadedData data;

    // ---- sites.csv: id,name,demand,urgency,lat,lon
    {
        Reporter report("sites.csv", data.problems);
        std::unordered_map<int, int> firstLine;  // site id -> line it was loaded from
        for (const auto& row : readDataRows(folder, "sites.csv", report)) {
            const auto& f = row.fields;
            const int line = row.lineNumber;
            if (f.size() != 6) {
                report.add(line, wrongFieldCount(6, f.size()));
                continue;
            }
            int id = 0, demand = 0;
            double urgency = 0.0, lat = 0.0, lon = 0.0;
            if (!parseInt(f[0], id)) { report.add(line, "id is not an integer"); continue; }
            if (!parseInt(f[2], demand)) { report.add(line, "demand is not a number"); continue; }
            if (!parseDouble(f[3], urgency)) { report.add(line, "urgency is not a number"); continue; }
            if (!parseDouble(f[4], lat)) { report.add(line, "lat is not a number"); continue; }
            if (!parseDouble(f[5], lon)) { report.add(line, "lon is not a number"); continue; }
            if (demand < 0) { report.add(line, "demand is negative"); continue; }
            if (urgency <= 0.0) { report.add(line, "urgency must be > 0"); continue; }
            const auto seen = firstLine.find(id);
            if (seen != firstLine.end()) {
                report.add(line, "duplicate site id " + std::to_string(id) +
                                     " (first seen on line " +
                                     std::to_string(seen->second) + ")");
                continue;
            }
            try {
                data.sites.emplace_back(id, f[1], demand, urgency, lat, lon);
            } catch (const std::invalid_argument& e) {  // safety net, checks above match
                report.add(line, e.what());
                continue;
            }
            firstLine[id] = line;
        }
    }

    // ---- depots.csv: id,name,supply,lat,lon
    {
        Reporter report("depots.csv", data.problems);
        std::unordered_map<int, int> firstLine;
        for (const auto& row : readDataRows(folder, "depots.csv", report)) {
            const auto& f = row.fields;
            const int line = row.lineNumber;
            if (f.size() != 5) {
                report.add(line, wrongFieldCount(5, f.size()));
                continue;
            }
            int id = 0, supply = 0;
            double lat = 0.0, lon = 0.0;
            if (!parseInt(f[0], id)) { report.add(line, "id is not an integer"); continue; }
            if (!parseInt(f[2], supply)) { report.add(line, "supply is not a number"); continue; }
            if (!parseDouble(f[3], lat)) { report.add(line, "lat is not a number"); continue; }
            if (!parseDouble(f[4], lon)) { report.add(line, "lon is not a number"); continue; }
            if (supply < 0) { report.add(line, "supply is negative"); continue; }
            const auto seen = firstLine.find(id);
            if (seen != firstLine.end()) {
                report.add(line, "duplicate depot id " + std::to_string(id) +
                                     " (first seen on line " +
                                     std::to_string(seen->second) + ")");
                continue;
            }
            try {
                data.depots.emplace_back(id, f[1], supply, lat, lon);
            } catch (const std::invalid_argument& e) {
                report.add(line, e.what());
                continue;
            }
            firstLine[id] = line;
        }
    }

    // ---- links.csv: depotId,siteId,capacity (only between loaded ids)
    {
        Reporter report("links.csv", data.problems);
        std::unordered_map<int, bool> depotLoaded, siteLoaded;
        for (const auto& d : data.depots) depotLoaded[d.id()] = true;
        for (const auto& s : data.sites) siteLoaded[s.id()] = true;
        std::set<std::pair<int, int>> linked;
        for (const auto& row : readDataRows(folder, "links.csv", report)) {
            const auto& f = row.fields;
            const int line = row.lineNumber;
            if (f.size() != 3) {
                report.add(line, wrongFieldCount(3, f.size()));
                continue;
            }
            int depotId = 0, siteId = 0, capacity = 0;
            if (!parseInt(f[0], depotId)) { report.add(line, "depotId is not an integer"); continue; }
            if (!parseInt(f[1], siteId)) { report.add(line, "siteId is not an integer"); continue; }
            if (!parseInt(f[2], capacity)) { report.add(line, "capacity is not a number"); continue; }
            if (capacity < 0) { report.add(line, "capacity is negative"); continue; }
            if (!depotLoaded.count(depotId)) {
                report.add(line, "unknown depot id " + std::to_string(depotId));
                continue;
            }
            if (!siteLoaded.count(siteId)) {
                report.add(line, "unknown site id " + std::to_string(siteId));
                continue;
            }
            if (!linked.insert({depotId, siteId}).second) {
                report.add(line, "duplicate link " + std::to_string(depotId) +
                                     " -> " + std::to_string(siteId));
                continue;
            }
            data.links.push_back({depotId, siteId, capacity});
        }
    }

    return data;
}
