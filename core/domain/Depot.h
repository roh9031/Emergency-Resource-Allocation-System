/* One warehouse that ships relief kits
 Holds the real id from depots.csv and how many kits it has (supply).
 node() is the nearest road node, -1 until snapped (iteration 4) */
#pragma once

#include <string>

class Depot {
public:
    // Throws std::invalid_argument if supply < 0
    // Time O(L) space O(L), L = name length
    Depot(int id, std::string name, int supply, double lat, double lon);

    // Time O(1) space O(1) for every getter
    int id() const;
    const std::string& name() const;
    int supply() const;
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
    int supply_;
    double lat_;
    double lon_;
    int node_ = -1;
};
