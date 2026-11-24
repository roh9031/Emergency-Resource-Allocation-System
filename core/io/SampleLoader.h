/* Iteration 3 prototype loader
 Reads <folder>/sites.csv, depots.csv and links.csv into validated Site and
 Depot objects plus the allowed depot->site links. A bad line is reported as
 "<file>:<line>: <message>" and skipped, never a crash. For a duplicate id
 the FIRST occurrence is kept. A link is kept only if both its depot and its
 site were loaded. In iteration 4 ScenarioLoader replaces this class and
 links come from travel time <= deadline instead of links.csv */
#pragma once

#include <string>
#include <vector>

#include "domain/Depot.h"
#include "domain/Site.h"

// One allowed depot -> site connection, real ids from the files
struct Link {
    int depotId;
    int siteId;
    int capacity;
};

struct LoadedData {
    std::vector<Site> sites;            // valid lines only, in file order
    std::vector<Depot> depots;
    std::vector<Link> links;
    std::vector<std::string> problems;  // one "<file>:<line>: <message>" per bad line
};

class SampleLoader {
public:
    // Throws std::runtime_error (naming the path) if any of the three files
    // cannot be opened. Bad lines never throw; they go to problems
    // Time O(C + S + D + K) expected, space O(C), C = characters in the
    // three files, S/D/K = number of site/depot/link lines
    LoadedData load(const std::string& folder) const;
};