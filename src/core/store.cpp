#include <iostream>
#include <optional>

#include "../../include/store.hpp"

namespace fluxobid {
    
std::optional<Campaign> CampaignStore::find_match(const Imp& imp, std::string_view country) const {
    auto it = _campaigns_by_country.find(country);
    if (it == _campaigns_by_country.end()) {
        return std::nullopt;
    }

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

}
