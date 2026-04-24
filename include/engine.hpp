#pragma once

#include <iostream>
#include <string_view>

#include "bidder.hpp"
#include "store.hpp"
#include "bid.hpp"

namespace fluxobid {

class Engine {
private:

public:
    std::vector<Bid> evaluate_request(const BidRequest& req, const CampaignStore& store);
};

} 
