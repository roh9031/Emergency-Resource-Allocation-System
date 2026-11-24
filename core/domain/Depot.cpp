#include "domain/Depot.h"

#include <stdexcept>
#include <utility>

// Time O(L) space O(L), L = name length
Depot::Depot(int id, std::string name, int supply, double lat, double lon)
    : id_(id), name_(std::move(name)), supply_(supply), lat_(lat), lon_(lon) {
    if (supply_ < 0) {
        throw std::invalid_argument("Depot: supply is negative");
    }
}

int Depot::id() const { return id_; }
const std::string& Depot::name() const { return name_; }
int Depot::supply() const { return supply_; }
double Depot::lat() const { return lat_; }
double Depot::lon() const { return lon_; }
int Depot::node() const { return node_; }

// Time O(1) space O(1)
void Depot::setNode(int node) {
    if (node < -1) {
        throw std::out_of_range("Depot::setNode: node index < -1");
    }
    node_ = node;
}
