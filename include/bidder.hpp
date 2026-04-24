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
    BidRequest current_request_;
    CampaignStore store_;

    void send_http_204(asio::ip::tcp::socket& socket);
    void send_http_200(asio::ip::tcp::socket& socket, const std::vector<Bid>& bids);

public:
    Bidder() = default;
    Bidder(const CampaignStore& store);

    bool parse_request(std::string_view json_raw);
    const BidRequest& get_current_request() const;
    RejectReason validate_request(const BidRequest& req);
    void handle_request(asio::ip::tcp::socket& socket, std::string_view json_raw);
};

}
