/* Common interface of the five allocation methods A1-A5
 The service layer stores allocators as AllocationStrategy pointers and runs
 any of them the same way (polymorphism)
 Header-only: an abstract class with no data and no method bodies */
#pragma once

#include <string>

#include "domain/Allocation.h"

/* Forward declaration this header only takes Scenario by reference so the
full class definition is not needed here */
class Scenario;

class AllocationStrategy {
public:
    // Virtual so deleting a subclass through an AllocationStrategy pointer
    // runs the subclass destructor
    virtual ~AllocationStrategy() = default;

    // Allocates the scenario's supply to its sites
    // Time and space depend on the subclass; each override documents its own
    virtual AllocationResult allocate(const Scenario& sc) const = 0;

    // Short method id, "A1" .. "A5"
    // Time O(1) space O(1)
    virtual std::string id() const = 0;

    
    // Time O(1) space O(1)
    virtual std::string name() const = 0;
};
