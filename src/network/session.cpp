#include <asio.hpp>
#include <memory>
#include <vector>

#include "../include/session.hpp"

namespace fluxobid {

void Session::read_request() {
    auto self = shared_from_this();
    socket_.async_read_some(asio::buffer(buffer_),
        [this, self](std::error_code ec, std::size_t length) {
            if (!ec) {
                std::string_view json_raw(buffer_.data(), length);
                
                Bidder bidder{store_};
                bidder.handle_request(socket_, json_raw);
            }
        });
}


Session::Session(asio::ip::tcp::socket socket, const CampaignStore& store) : socket_(std::move(socket)), store_(store) {}

void Session::start() {
    read_request();
}

}
