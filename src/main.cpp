#include <asio.hpp>
#include <iostream>

#include "../include/bidder.hpp"
#include "../include/engine.hpp"
#include "../include/reject_reason.hpp"
#include "../include/metrics.hpp"
#include "../include/store.hpp"

//import fluxobid.bidder;

/**
 * A simple coroutine that represents the starting point of my engine.
 * The 'asio::awaitable<void>' return type is what allows this function to be paused and resumed by the Asio event loop.
 */
asio::awaitable<void> start_engine() {
    // In a real adtech app, this is where you'd initialize your campaign cache and targeting rules.
    std::cout << "FluxoBid Engine: Initializing high-speed bidding core..." << std::endl;
    
    // We 'co_await' a timer to simulate a non-blocking startup delay
    asio::steady_timer timer(co_await asio::this_coro::executor, std::chrono::milliseconds(100));
    co_await timer.async_wait(asio::use_awaitable);

    std::cout << "FluxoBid Engine Started [C++20 Coroutines Active]" << std::endl;
}

int main() {
    try {
        // The io_context is the "engine room" that manages all async tasks
        asio::io_context ctx;

        fluxobid::Metrics metrics; // Instantiate a Metrics object here and pass a reference to it into my Bidder class.

        fluxobid::CampaignStore store;


        // 'co_spawn' attaches our coroutine to the context's executor 'asio::detached' means we don't need to wait for the result here
        asio::co_spawn(ctx, start_engine(), asio::detached);

        // This blocks until all asynchronous work is complete
        ctx.run();
    } 
    catch (std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
