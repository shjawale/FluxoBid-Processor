#pragma once

#include <string_view>
#include <unordered_map>

namespace fluxobid {

class GeoProvider {
private:
    std::unordered_map<std::string_view, std::string_view> ip_map_;
    
public:
    GeoProvider();

    std::string_view lookup(std::string_view ip) const;
};

}
