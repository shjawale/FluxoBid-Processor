#pragma once

#include <string_view>
#include <vector>
#include <optional>
#include <iostream>
#include <string>
#include <asio.hpp>

#include "reject_reason.hpp"
#include "store.hpp"
#include "bid.hpp"
#include "models.hpp"

namespace fluxobid {

class Bidder {
private:
    BidRequest _current_request;
    CampaignStore _store;

public:
    bool parse_request(std::string_view json_raw);
    const BidRequest& get_current_request() const;
    RejectReason validate_request(const BidRequest& req);
    void handle_request(std::string_view json_raw);
    
    void send_http_204(asio::ip::tcp::socket& socket);
    void send_bid_response(asio::ip::tcp::socket& socket, const std::vector<Bid>& bids);
};

}
