#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <yyjson.h>

#include "../include/engine.hpp"

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

    for (const auto& imp : req.imps) {
        auto best_campaign = store.find_match(imp, req.device_ip);

        if (best_campaign) {
            if (best_campaign->price >= imp.bidfloor.value_or(0.0)) {
                wins[i] = Bid(imp.id, best_campaign->price, best_campaign->ad_id);
                i++;
            }
        }
    }
    return wins;
}

}
