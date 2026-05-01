#include <string_view>
#include <unordered_map>

#include "../../include/geoprovider.hpp"

namespace fluxobid {
GeoProvider::GeoProvider() {
    ip_map_["127.0.0.1"] = "LOCAL";
    ip_map_["1.1.1.1"]   = "AU";
    ip_map_["8.8.8.8"]   = "US";
}

std::string_view GeoProvider::lookup(std::string_view ip) const {
    auto it = ip_map_.find(ip);
    if (it != ip_map_.end()) {
        return it->second;
    }
    return "UNKNOWN";
}
}
