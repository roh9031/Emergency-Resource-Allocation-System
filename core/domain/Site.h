#pragma once

#include <string>

class Site {
public:
    // Throws std::invalid_argument if demand < 0 or urgency <= 0
    // (NaN urgency is rejected too)
    // Time O(L) space O(L), L = name length
    Site(int id, std::string name, int demand, double urgency, double lat, double lon);

    // Time O(1) space O(1) for every getter
    int id() const;
    const std::string& name() const;
    int demand() const;
    double urgency() const;
    double lat() const;
    double lon() const;

    // Nearest road node, -1 until snapped
    int node() const;

    // Throws std::out_of_range if node < -1 (-1 means "not snapped")
    // Time O(1) space O(1)
    void setNode(int node);

private:
    int id_;
    std::string name_;
    int demand_;
    double urgency_;
    double lat_;
    double lon_;
    int node_ = -1;
};
