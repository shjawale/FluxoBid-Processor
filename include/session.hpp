#pragma once

#include <asio.hpp>
#include <memory>
#include <vector>

#include "bidder.hpp"
#include "store.hpp"

namespace fluxobid {

class Session : public std::enable_shared_from_this<Session> {
private:
    asio::ip::tcp::socket socket_;
    const CampaignStore& store_;
    std::array<char, 4096> buffer_;

    void read_request();

public:
    explicit Session(asio::ip::tcp::socket socket, const CampaignStore& store);
    void start(); 
};

}
