#include <asio.hpp>
#include <iostream>
#include <string_view>

#include "../include/bidder.hpp"
#include "../include/store.hpp"
#include "../include/server.hpp"


asio::awaitable<void> handle_session(asio::ip::tcp::socket socket, const fluxobid::CampaignStore& store) {
    try {
        char buffer[4096];
        std::size_t n = co_await socket.async_read_some(asio::buffer(buffer), asio::use_awaitable);
        std::string_view json_raw(buffer, n);

        fluxobid::Bidder bidder(store);
        
        bidder.handle_request(socket, json_raw);

    } catch (std::exception& e) {
        std::cerr << "Session Error: " << e.what() << std::endl;
    }
}


asio::awaitable<void> server_listener(const fluxobid::CampaignStore& store) {
    auto executor = co_await asio::this_coro::executor;
    asio::ip::tcp::acceptor acceptor(executor, {asio::ip::tcp::v4(), 8080});
    
    std::cout << "[FluxoBid] Listening on port 8080..." << std::endl;

    for (;;) {
        asio::ip::tcp::socket socket = co_await acceptor.async_accept(asio::use_awaitable);
        
        asio::co_spawn(executor, handle_session(std::move(socket), store), asio::detached);
    }
}

int main() {
    asio::io_context io_context;
    
    fluxobid::CampaignStore store;

    fluxobid::Server server{io_context, 8080, store};

    io_context.run();
    return 0;
}

