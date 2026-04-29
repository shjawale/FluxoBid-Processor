#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <yyjson.h>

#include "../../include/engine.hpp"
#include "../../include/geoprovider.hpp"

namespace fluxobid {

bool Engine::check_country(const Campaign& campaign, std::string_view target_country) const {
    return campaign.target_country == target_country;
}


std::vector<Bid> Engine::evaluate_request(const BidRequest& req, const CampaignStore& store, std::string_view target_country) {
    std::vector<Bid> wins;
    wins.resize(req.imps.size());
    int i = 0;

    for (const auto& imp : req.imps) {
        static GeoProvider geo; 
        std::string_view device_country = geo.lookup(req.device_ip);
        auto best_campaign = store.find_match(imp, device_country);

        if (best_campaign != std::nullopt) {     
            if (Engine::check_country(*best_campaign, target_country)) {
                if (best_campaign->price >= imp.bidfloor.value_or(0.0)) {
                    wins[i] = Bid(imp.id, best_campaign->price, best_campaign->ad_id); //works
                    i++;
                }
            }
        }
    }
    return wins;
}

}
