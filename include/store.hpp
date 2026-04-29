#pragma once

#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <string_view>
#include <cstdint>

#include "models.hpp"

namespace fluxobid {

struct Campaign {
    uint32_t campaign_id;
    double price;
    std::string_view target_country;
    int width;
    int height;
    std::string_view ad_id;
    std::string_view ad_markup;

    Campaign() = default;
    Campaign(uint32_t camp_id, double price, std::string_view country, int width, int height);
    
    void printCampaign();
};

class CampaignStore {
private:
    std::vector<Campaign> campaigns_;
    std::unordered_map<std::string_view, std::vector<Campaign>> campaigns_by_country_;

public:
    std::optional<Campaign> find_match(const Imp& imp, std::string_view country) const;
    void add_campaign(const Campaign& camp);
};
}
