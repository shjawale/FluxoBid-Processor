/*
#ifndef __INTELLISENSE__
module;
#endif
*/

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <yyjson.h>

#include "../../include/engine.hpp"
#include "../../include/geoprovider.hpp"

/*
#ifndef __INTELLISENSE__
export module fluxobid.engine;
#endif
*/

namespace fluxobid {
/*
std::vector<Bid> Engine::evaluate_request(const BidRequest& req, const CampaignStore& store) {
    std::vector<Bid> wins;
    wins.resize(req.imps.size());
    int i = 0;

    // Iterate through every impression slot in the request
    for (const auto& imp : req.imps) {
        // Find the best campaign for THIS specific impression
        auto best_campaign = store.find_match(imp, req.device_ip);

        if (best_campaign) {        // If the campaign's price is higher than the floor, we bid
            if (best_campaign->price >= imp.bidfloor.value_or(0.0)) {
                wins[i] = Bid(imp.id, best_campaign->price, best_campaign->ad_id); //works
                i++;
            }
        }
    }
    return wins;
}
*/

bool Engine::check_country(const Campaign& campaign, std::string_view target_country) const {
    // Basic string comparison is fast, but this is where you'd later add more complex logic (like "All Countries" or "Exclusion Lists")
    return campaign.target_country == target_country;
}


std::vector<Bid> Engine::evaluate_request(const BidRequest& req, const CampaignStore& store, std::string_view target_country) {
    std::vector<Bid> wins(req.imps.size());
    int i = 0;
    std::cout << "req.imps[0].id: " << req.imps[0].id << '\n';     //"bid_floor: " << req.imps[0].bidfloor << '\n';

    // Iterate through every impression slot in the request
    for (const auto& imp : req.imps) {
        // Find the best campaign for THIS specific impression
        static GeoProvider geo; 
        std::string_view device_country = geo.lookup(req.device_ip);
        auto best_campaign = store.find_match(imp, device_country);

        //std::cout << typeid(Campaign()).name();
        //if (typeid(best_campaign) == typeid(std::optional<Campaign>())) {
        //best_campaign->printCampaign();
        std::cout << '\n';
        //}

        if (best_campaign != std::nullopt) {        // If the campaign's price is higher than the floor, we bid
            std::cout << "best_campaign exists\n";
            if (Engine::check_country(*best_campaign, target_country)) {
                std::cout << "in evaluate_req: ad_id: " << best_campaign->ad_id << "  imp_id: " << imp.id << "  price: " << best_campaign->price << '\n';
                if (best_campaign->price >= imp.bidfloor.value_or(0.0)) {
                    wins[i] = Bid(imp.id, best_campaign->price, best_campaign->ad_id); //works
                    std::cout << "wins[i].ad_id: " << wins[i].ad_id << '\n';
                    i++;
                    //wins.push_back(Bid(imp.id, best_campaign->price, best_campaign->ad_id));
                }
            }
        }
        std::cout << "end of iteration\n";
    }
    std::cout << "after for loop\n";
    std::cout << "wins[0].ad_id: " << wins[0].ad_id << '\n';
    return wins;
}

}
