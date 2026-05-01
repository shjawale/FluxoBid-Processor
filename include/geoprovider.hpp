#pragma once

#include <string_view>
#include <unordered_map>

namespace fluxobid {

class GeoProvider {
private:
    // Map of IP strings to ISO Country Codes
    std::unordered_map<std::string_view, std::string_view> ip_map_;
    
public:
    // In a real app, this would load a CSV or binary file into the map
    GeoProvider();

    // Standard O(1) lookup
    std::string_view lookup(std::string_view ip) const;
};

}
