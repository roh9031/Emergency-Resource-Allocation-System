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

    /* One line per method: id, name, kits delivered, urgency-weighted value
       (sum over shipments of units x urgency of the receiving site) and runtime.
       results[i] is described by names[i]
       Time O(M (K + S)) expected, space O(S), M = number of results */
    static void printComparison(const std::vector<AllocationResult>& results,
                                const std::vector<std::string>& names,
                                const LoadedData& data, std::ostream& out = std::cout);

    /* Sum over shipments of units x urgency of the receiving site; shipments to
       a site id not in data count 0. Rewards serving urgent sites first
       Time O(K + S) expected, space O(S) */
    static double weightedValue(const AllocationResult& result, const LoadedData& data);

    // Prints each "<file>:<line>: <message>" or a line saying there were none
    // Time O(P) space O(1), P = number of problems
    static void printProblems(const std::vector<std::string>& problems,
                              std::ostream& out = std::cout);

    // One line: "<n> sites, <n> depots, <n> links, <n> problem lines"
    // Time O(1) space O(1)
    static void printLoadSummary(const std::string& folder, const LoadedData& data,
                                 std::ostream& out = std::cout);
};
