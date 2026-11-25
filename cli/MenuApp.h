/* Text menu for the iteration 3 prototype
   1. Load a sample folder (default data/sample/basic)
   2. Show load problems
   3. Run allocation -> asks which method:
        1. A1 greedy (urgency first)
        2. A2 max flow
        3. Compare A1 vs A2   (runs both, one summary line each)
   4. Exit
 The methods are passed in, so MenuApp does not depend on which allocators
 exist (main.cpp builds the list; AllocationService replaces it in iteration 4).
 An empty list means "no allocator built in": option 3 says so instead of
 crashing. Every error (missing folder, allocator exception, bad choice) is
 printed and the menu continues */
#pragma once

#include <cstddef>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "domain/Allocation.h"
#include "io/SampleLoader.h"

class MenuApp {
public:
    using AllocateFn = std::function<AllocationResult(const LoadedData&)>;

    // One allocation method offered in the menu
    struct Method {
        std::string id;        // "A1", "A2", ...
        std::string name;      // shown in the menu, e.g. "greedy (urgency first)"
        AllocateFn allocate;
    };

    // Time O(1) space O(1) (the method list is moved in)
    MenuApp(std::vector<Method> methods, std::istream& in = std::cin,
            std::ostream& out = std::cout);

    // Runs until the user picks Exit or input ends. Returns 0
    // Time: one iteration per menu choice; each choice costs what it runs
    int run();

    // Loads `folder` and prints a one-line summary; on a missing file prints
    // the error and keeps any previously loaded data. Returns true on success
    // Time O(C + S + D + K), see SampleLoader::load
    bool loadFolder(const std::string& folder);

    // Asks which method (or compare) and runs it. Returns false (with a message)
    // if nothing is loaded, no method is built in, input ends, or it fails
    bool runAllocation();

    // Runs method `index` (0-based) on the loaded data, times it and prints the
    // table. Returns false (with a message) if nothing is loaded, the index is
    // out of range, or the allocator throws
    // Time: the allocator's cost plus O(K + S + D) to print
    bool runMethod(std::size_t index);

    // Runs every method on the loaded data and prints one comparison line each
    // (kits delivered, urgency-weighted value, runtime). With showTables, the
    // full table of each method is printed first. Returns false if nothing is
    // loaded, no method exists, or any allocator throws (the others still run)
    // Time: the sum of the allocators' costs plus O(M (K + S)) to print
    bool compareAll(bool showTables = false);

    // Time O(P) space O(1)
    void showProblems() const;

private:
    // Runs one method and times it; throws whatever the allocator throws
    AllocationResult timedRun(const Method& method) const;

    std::vector<Method> methods_;
    std::istream& in_;
    std::ostream& out_;
    std::optional<LoadedData> data_;  // empty until a folder loads
    std::string folder_;
};
