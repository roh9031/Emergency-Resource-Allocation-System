#include "domain/Allocation.h"

#include <stdexcept>
#include <utility>
#include <cmath>
// Time O(L) space O(L), L = methodId length
AllocationResult::AllocationResult(std::string methodId)
    : methodId_(std::move(methodId)) {
    if (methodId_.empty()) {
        throw std::invalid_argument("AllocationResult: methodId is empty");
    }
}

// Time O(1) amortized space O(1) amortized
void AllocationResult::add(int depotId, int siteId, int units) {
    // Validate before changing any member
    if (depotId < 0) {
        throw std::invalid_argument("AllocationResult::add: negative depotId");
    }

    if (siteId < 0) {
        throw std::invalid_argument("AllocationResult::add: negative siteId");
    }

    if (units < 0) {
        throw std::invalid_argument("AllocationResult::add: negative units");
    }

    // Zero units do not represent an actual shipment
    if (units == 0) {
        return;
    }

    // Store the shipment
    shipments_.push_back({depotId, siteId, units});

    // Update total units allocated
    totalUnits_ += units;
}

// Time O(1) space O(1)
const std::vector<Allocation>& AllocationResult::shipments() const {
    return shipments_;
}

// Time O(1) space O(1)
long long AllocationResult::totalUnits() const {
    return totalUnits_;
}

// Time O(1) space O(1)
const std::string& AllocationResult::methodId() const {
    return methodId_;
}

// Time O(1) space O(1)
double AllocationResult::runtimeMs() const {
    return runtimeMs_;
}

// Time O(1) space O(1)
void AllocationResult::setRuntimeMs(double ms) {
    if (!std::isfinite(ms) || ms < 0.0) {
        throw std::invalid_argument(
            "AllocationResult::setRuntimeMs: invalid runtime"
        );
    }

    runtimeMs_ = ms;
}
