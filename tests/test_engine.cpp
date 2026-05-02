#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <iostream>

#include "../include/engine.hpp"
#include "../include/store.hpp"
#include "../include/models.hpp"

TEST_CASE("Engine Auction Logic", "[engine][auction]") {
    fluxobid::Engine engine;
    fluxobid::CampaignStore store;

    fluxobid::Campaign campA{101, "nike_summer_01", 1.00, "US", 300, 250};
    store.add_campaign(campA);

    fluxobid::Campaign campB{102, "adidas_summer_02", 2.00, "US", 300, 250};
    store.add_campaign(campB);

    fluxobid::Campaign campC{103, "asic_summer_02", 5.00, "UK", 300, 250};
    store.add_campaign(campC);

    SECTION("Engine picks the highest-paying valid campaign") {
        std::vector<fluxobid::Imp> newImp{{"slot_1", 0.50}};
        fluxobid::BidRequest req{"req_1", newImp, "8.8.8.8"};
        
        auto bids = engine.evaluate_request(req, store, "US");

        REQUIRE(bids.size() == 1);
        std::cout << "bids[0].ad_id: " << bids[0].ad_id << '\n';
        REQUIRE(bids[0].ad_id == "adidas_summer_02");
        REQUIRE(bids[0].price == 2.00);
    }

    SECTION("Engine respects the Bid Floor") {
        std::vector<fluxobid::Imp> newImp{{"slot_1", 3.00}};
        fluxobid::BidRequest req{"req_2", newImp, "1.1.1.1"};
        
        auto bids = engine.evaluate_request(req, store, "US");

        REQUIRE(bids.empty()); 
    }

    SECTION("Engine handles multiple impressions independently") {
        std::cout << "started test 3...\n";
        std::vector<fluxobid::Imp> newImp{{"us_slot", 0.10}, {"uk_slot", 0.10}};
        fluxobid::BidRequest req{"req_2", newImp, "8.8.8.8"};

        auto bids = engine.evaluate_request(req, store, "US"); 
        
        REQUIRE(bids.size() == 2);
        REQUIRE(bids[0].imp_id == "us_slot");
    }

    SECTION("Engine returns nothing when no geography matches") {
        std::vector<fluxobid::Imp> newImp{{"slot_1", 0.0}};
        fluxobid::BidRequest req{"req_2", newImp, "8.8.8.8"};

        auto bids = engine.evaluate_request(req, store, "FR");

        REQUIRE(bids.empty());
    }
}
