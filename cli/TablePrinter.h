/* Formats allocation results for the terminal
 printAllocation: aligned table of shipments (depot -> site -> units) with
 names from the loaded data, then a summary: total supply, total demand,
 kits delivered, and the fill rate (delivered / demand) of every site.
 Output stream is a parameter (default std::cout) so tests can capture it */
#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "domain/Allocation.h"
#include "io/SampleLoader.h"

class TablePrinter {
public:
    // Time O(K + S + D) expected, space O(S + D), K = shipments,
    // S = sites, D = depots
    static void printAllocation(const AllocationResult& result, const LoadedData& data,
                                std::ostream& out = std::cout);

    // Prints each "<file>:<line>: <message>" or a line saying there were none
    // Time O(P) space O(1), P = number of problems
    static void printProblems(const std::vector<std::string>& problems,
                              std::ostream& out = std::cout);

    // One line: "<n> sites, <n> depots, <n> links, <n> problem lines"
    // Time O(1) space O(1)
    static void printLoadSummary(const std::string& folder, const LoadedData& data,
                                 std::ostream& out = std::cout);
};
