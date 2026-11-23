/* Text menu for the iteration 3 prototype (interfaces.md §5)
   1. Load a sample folder (default data/sample/basic)
   2. Show load problems
   3. Run allocation
   4. Exit
 The allocator is passed in as a function so MenuApp does not depend on
 which allocator exists yet (A2 now, AllocationService in iteration 4).
 An empty function means "no allocator built in": option 3 says so instead
 of crashing. Every error (missing folder, allocator exception) is printed
 and the menu continues */
#pragma once

#include <functional>
#include <iostream>
#include <optional>
#include <string>

#include "domain/Allocation.h"
#include "io/SampleLoader.h"

class MenuApp {
public:
    using AllocateFn = std::function<AllocationResult(const LoadedData&)>;

    // Time O(1) space O(1)
    MenuApp(AllocateFn allocate, std::istream& in = std::cin, std::ostream& out = std::cout);

    // Runs until the user picks Exit or input ends. Returns 0
    // Time: one iteration per menu choice; each choice costs what it runs
    int run();

    // Loads `folder` and prints a one-line summary; on a missing file prints
    // the error and keeps any previously loaded data. Returns true on success
    // Time O(C + S + D + K), see SampleLoader::load
    bool loadFolder(const std::string& folder);

    // Runs the allocator on the loaded data and prints the table. Returns
    // false (with a message) if nothing is loaded, no allocator is built in,
    // or the allocator throws
    // Time: the allocator's cost plus O(K + S + D) to print
    bool runAllocation();

    // Time O(P) space O(1)
    void showProblems() const;

private:
    AllocateFn allocate_;
    std::istream& in_;
    std::ostream& out_;
    std::optional<LoadedData> data_;  // empty until a folder loads
    std::string folder_;
};
