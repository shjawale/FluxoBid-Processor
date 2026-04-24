#pragma once

#include <iostream>
#include <asio.hpp>
#include <cstdint>

#include "store.hpp"

namespace fluxobid {

class Server {
private:
    asio::ip::tcp::acceptor acceptor_;
    const CampaignStore& store_;

    void do_accept();

public:
    Server() = delete;
    Server(asio::io_context& io_context, unsigned short port, const CampaignStore& store);
};

}
