/* Output of every allocation method (A1-A5)
 Allocation is one shipment depot to site a number of kits using the REAL
 ids from the input files (not internal 0..m-1 / 0..n-1 indices)
 AllocationResult collects the shipments of one run together with the method
 id and its runtimemetrics writers the CLI and the server read it */
#pragma once

#include <string>
#include <vector>

// Plain record with public fields on purpose it has no invariants of its
// own AllocationResult::add validates every value before storing one
struct Allocation {
    int depotId;
    int siteId;
    int units;
};

class AllocationResult {
public:
    // Throws std::invalid_argument if methodId is empty
    // Time O(l) space O(L), L = methodId length
    explicit AllocationResult(std::string methodId);

    /* for my ref: records that depotId ships `units` kits to siteId
     units == 0 is ignored (no shipment is stored) so allocators can call
     add for every link without filtering
     Throws std::invalid_argument if depotId < 0 siteId < 0 or units < 0
     the result is unchanged when it throws
     The same (depot site) pair added twice is kept as TWO entries;
     AllocationValidator reports duplicates
     Time O(1) amortized space O(1) amortized */    
    void add(int depotId, int siteId, int units);

    // Shipments in the order they were added
    // Time O(1) space O(1)
    const std::vector<Allocation>& shipments() const;

    // Sum of units over all shipments (running total long long so many
    // int units cannot overflow)
    // Time O(1) space O(1)
    long long totalUnits() const;

    // Time O(1) space O(1)
    const std::string& methodId() const;

    // Wall clock time of the run in milliseconds 0 until set
    // Time O(1) space O(1)
    double runtimeMs() const;

    // Throws std::invalid_argument if ms is negative or NaN or infinity
    // Time O(1) space O(1)
    void setRuntimeMs(double ms);

private:
    std::string methodId_;
    std::vector<Allocation> shipments_;
    long long totalUnits_ = 0;
    double runtimeMs_ = 0.0;
};
