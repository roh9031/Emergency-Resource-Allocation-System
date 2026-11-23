#include "domain/Params.h"

#include <stdexcept>

// Time O(1) space O(1)
Params::Params(double deadlineMinutes, int linkCapacity, double alpha, unsigned seed)
    : deadline_(deadlineMinutes), linkCapacity_(linkCapacity), alpha_(alpha),
      seed_(seed) {
    // !(x >= 0) also rejects NaN; +infinity deadline is allowed (no limit)
    if (!(deadline_ >= 0.0)) {
        throw std::invalid_argument("Params: deadline must be >= 0");
    }
    if (linkCapacity_ < 0) {
        throw std::invalid_argument("Params: linkCapacity must be >= 0");
    }
    if (!(alpha_ >= 0.0)) {
        throw std::invalid_argument("Params: alpha must be >= 0");
    }
}

double Params::deadline() const { return deadline_; }
int Params::linkCapacity() const { return linkCapacity_; }
double Params::alpha() const { return alpha_; }
unsigned Params::seed() const { return seed_; }
