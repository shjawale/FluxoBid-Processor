#include <iostream>
#include <optional>

#include "../../include/store.hpp"

namespace fluxobid {

Campaign::Campaign(uint32_t campaign_id, std::string_view ad_id, double price, std::string_view country, int width, int height) : campaign_id(campaign_id), ad_id(ad_id), price(price), target_country(country), width(width), height(height) {}

void Campaign::printCampaign() {
    std::cout << "printing campaign... campaign_id: " << campaign_id 
    << "  price: " << price 
    << "  target_country: " << target_country 
    << "  width: " << width << "  height: " << height
    << "  ad_id: " << ad_id 
    << "  ad_markup: " << ad_markup << '\n';
}

    
std::optional<Campaign> CampaignStore::find_match(const Imp& imp, std::string_view country) const {
    std::cout << "in find_match...  ";
    std::cout << "country: " << country << '\n';

    auto it = campaigns_by_country_.find(country);
    if (it == campaigns_by_country_.end()) {
        return std::nullopt;
    }
    std::cout << "it: " << typeid(it->first).name() << '\n';

    const std::vector<Campaign>& candidates = it->second;
    const Campaign* best_candidate = nullptr;

    for (const auto& campaign : candidates) {
        if (campaign.price < imp.bidfloor.value_or(0.0)) {
            continue;
        }

        if (imp.banner.has_value()) {
            if (campaign.width != imp.banner->w || campaign.height != imp.banner->h) {
                continue;
            }
        }

        if (!best_candidate || campaign.price > best_candidate->price) {
            best_candidate = &campaign;
        }
    }

    if (best_candidate) {
        return *best_candidate;
    }

    return std::nullopt;
}

void CampaignStore::add_campaign(const Campaign& camp) {
    campaigns_.push_back(camp);
    campaigns_by_country_[camp.target_country].push_back(camp);

    campaigns_.back().printCampaign();
}

}
