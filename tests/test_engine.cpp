#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <iostream>

#include "../include/engine.hpp"
#include "../include/store.hpp"
#include "../include/models.hpp"

TEST_CASE("Engine Auction Logic", "[engine][auction]") {
    fluxobid::Engine engine;
    fluxobid::CampaignStore store;

    fluxobid::Campaign campA{'A', 1.00, "US", 300, 250};
    store.add_campaign(campA);

    fluxobid::Campaign campB{'B', 2.00, "US", 300, 250};
    store.add_campaign(campB);

    fluxobid::Campaign campC{'C', 5.00, "UK", 300, 250};
    store.add_campaign(campC);

    SECTION("Engine picks the highest-paying valid campaign") {
        fluxobid::BidRequest req;
        req.device_ip = "1.1.1.1"; 
        req.imps.push_back({.id = "slot_1", .bidfloor = 0.50});
        
        auto bids = engine.evaluate_request(req, store, "US");

        REQUIRE(bids.size() == 1);
        REQUIRE(bids[0].ad_id == "B");
        REQUIRE(bids[0].price == 2.00);
    }

    SECTION("Engine respects the Bid Floor") {
        fluxobid::BidRequest req;
        req.imps.push_back({.id = "slot_1", .bidfloor = 3.00}); 
        
        auto bids = engine.evaluate_request(req, store, "US");

        REQUIRE(bids.empty()); 
    }

    SECTION("Engine handles multiple impressions independently") {
        fluxobid::BidRequest req;
        req.imps.push_back({.id = "us_slot", .bidfloor = 0.10});
        req.imps.push_back({.id = "uk_slot", .bidfloor = 0.10});

        auto bids = engine.evaluate_request(req, store, "US"); 
        
        REQUIRE(bids.size() == 1);
        REQUIRE(bids[0].imp_id == "us_slot");
    }

    SECTION("Engine returns nothing when no geography matches") {
        fluxobid::BidRequest req;
        req.imps.push_back({.id = "slot_1"});
        
        auto bids = engine.evaluate_request(req, store, "FR");

        REQUIRE(bids.empty());
    }
}
