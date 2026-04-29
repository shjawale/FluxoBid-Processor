#pragma once

#include <iostream>
#include <string_view>

#include "bidder.hpp"
#include "store.hpp"
#include "bid.hpp"

namespace fluxobid {

class Engine {
private:
    bool check_country(const Campaign& campaign, std::string_view target_country) const;
public:
    std::vector<Bid> evaluate_request(const BidRequest& req, const CampaignStore& store, std::string_view target_country);
};

} 
