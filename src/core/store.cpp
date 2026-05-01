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
    // Quick Lookup: Get all campaigns for this country
    // Using a map lookup is O(1), much faster than looping over all campaigns.
    std::cout << "in find_match...  ";
    std::cout << "country: " << country << '\n';

    auto it = campaigns_by_country_.find(country);
    if (it == campaigns_by_country_.end()) {
        return std::nullopt;
    }
    std::cout << "it: " << typeid(it->first).name() << '\n';      //' ' << it->second.size() << '\n'; // typeid(it).name() << '\n';

    const std::vector<Campaign>& candidates = it->second;
    const Campaign* best_candidate = nullptr;

    // Filter & Auction Loop
    for (const auto& campaign : candidates) {
        // A. Floor Price Check
        // If the campaign pays less than the floor, it's an automatic disqualification.
        if (campaign.price < imp.bidfloor.value_or(0.0)) {
            continue;
        }

        // B. Size Matching (Example for Banner)
        // If the request is for a 300x250 and our ad is 320x50, we skip.
        if (imp.banner.has_value()) {
            if (campaign.width != imp.banner->w || campaign.height != imp.banner->h) {
                continue;
            }
        }

        // C. Highest Bidder Logic (Internal Auction)
        // We want to return the campaign that makes us the most money.
        if (!best_candidate || campaign.price > best_candidate->price) {
            best_candidate = &campaign;
        }
    }

    if (best_candidate) {
        return *best_candidate; // Return a copy of the winner
    }

    return std::nullopt;
}

void CampaignStore::add_campaign(const Campaign& camp) {
    // Organizes ads by country in an unordered_map for O(1) lookup during bidding.
    // This creates a bucket for each country (e.g., "US", "UK", "DE").

    //Campaign campA('A', 1.00, "US", 300, 250);
    campaigns_.push_back(camp);
    campaigns_by_country_[camp.target_country].push_back(camp);

    campaigns_.back().printCampaign();
    std::cout << '\n';
    std::cout << "campaigns_.size(): " << campaigns_.size() << '\n';
    std::cout << "\n\n";
}

}
