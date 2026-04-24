#include <iostream>
#include <optional>

#include "../../include/store.hpp"

namespace fluxobid {
    
std::optional<Campaign> CampaignStore::find_match(const Imp& imp, std::string_view country) const {
    // Quick Lookup: Get all campaigns for this country
    // Using a map lookup is O(1), much faster than looping over all campaigns.
    auto it = _campaigns_by_country.find(country);
    if (it == _campaigns_by_country.end()) {
        return std::nullopt;
    }

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

}
