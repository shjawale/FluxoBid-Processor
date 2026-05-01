#include <asio.hpp>
#include <memory>
#include <vector>

#include "../../include/session.hpp"

namespace fluxobid {

void Session::read_request() {
    auto self = shared_from_this();
    std::cout << "Received request\n";
    
    // Use a fixed buffer for performance; 4KB is standard for OpenRTB
    socket_.async_read_some(asio::buffer(buffer_),
        [this, self](std::error_code ec, std::size_t length) {
            if (!ec) {
                std::string_view json_raw(buffer_.data(), length);
                
                // Business Logic: Hand off to Bidder
                // We pass 'socket_' so the bidder can write the response back
                Bidder bidder{store_};
                bidder.handle_request(socket_, json_raw);
            }
        });
}


Session::Session(asio::ip::tcp::socket socket, const CampaignStore& store) : socket_(std::move(socket)), store_(store) {}

void Session::start() {
    std::cout << "Session started\n";
    read_request();
}

}
