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

#include "../include/engine.hpp"

/*
#ifndef __INTELLISENSE__
export module fluxobid.engine;
#endif
*/

namespace fluxobid {

Bid::Bid(const std::string_view imp_id, double price, std::string_view ad_id) {
    _imp_id = imp_id;
    _price = price;
    _ad_id = ad_id;
}


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
                /*
                Bid bid{imp.id, best_campaign->price, best_campaign->ad_id};    //works
                wins.push_back(bid);
                */
                ///*
                wins[i] = Bid(imp.id, best_campaign->price, best_campaign->ad_id); //works
                i++;
                //*/
            }
        }
    }
    return wins;
}

}
