#include <iostream>
#include <asio.hpp>
#include <cstdint>

#include "../../include/server.hpp"
#include "../../include/session.hpp"

namespace fluxobid {

void Server::do_accept() {
    acceptor_.async_accept(
        [this](std::error_code ec, asio::ip::tcp::socket socket) {
            if (!ec) {
                // Create Session, hand off the socket, and start it
                std::make_shared<Session>(std::move(socket), store_)->start();
            }
            // Immediately listen for the next connection
            std::cout << "Accepted connection\n";
            do_accept();
        });
}


Server::Server(asio::io_context& io_context, unsigned short port, const CampaignStore& store) : acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)), store_(store) {
    do_accept();
}

}
