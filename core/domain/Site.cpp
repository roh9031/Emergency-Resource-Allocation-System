#include "domain/Site.h"

#include <stdexcept>
#include <utility>

// Time O(L) space O(L), L = name length
Site::Site(int id, std::string name, int demand, double urgency, double lat, double lon)
    : id_(id), name_(std::move(name)), demand_(demand), urgency_(urgency),
      lat_(lat), lon_(lon) {
    if (demand_ < 0) {
        throw std::invalid_argument("Site: demand is negative");
    }
    // Written as !(urgency > 0) so NaN is rejected as well
    if (!(urgency_ > 0.0)) {
        throw std::invalid_argument("Site: urgency must be > 0");
    }
}

int Site::id() const { return id_; }
const std::string& Site::name() const { return name_; }
int Site::demand() const { return demand_; }
double Site::urgency() const { return urgency_; }
double Site::lat() const { return lat_; }
double Site::lon() const { return lon_; }
int Site::node() const { return node_; }

// Time O(1) space O(1)
void Site::setNode(int node) {
    if (node < -1) {
        throw std::out_of_range("Site::setNode: node index < -1");
    }
    node_ = node;
}
