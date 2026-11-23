/* Run settings shared by every allocator
 deadline (tau): a depot may serve a site only if travel time <= deadline
 linkCapacity: most kits on one depot->site link, 0 = unlimited
 alpha: weight of travel time in the min-cost objective (0 in main experiments)
 seed: random seed for InstanceGenerator */
#pragma once

class Params {
public:
    // Throws std::invalid_argument if deadlineMinutes < 0, linkCapacity < 0
    // or alpha < 0 (NaN deadline or alpha is rejected too)
    // Time O(1) space O(1)
    Params(double deadlineMinutes, int linkCapacity, double alpha, unsigned seed);

    // Time O(1) space O(1) for every getter
    double deadline() const;   // tau, minutes
    int linkCapacity() const;  // 0 = unlimited
    double alpha() const;
    unsigned seed() const;

private:
    double deadline_;
    int linkCapacity_;
    double alpha_;
    unsigned seed_;
};
