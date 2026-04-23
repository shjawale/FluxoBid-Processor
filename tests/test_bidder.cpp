#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include "../include/bidder.hpp"
#include <iostream>

uint32_t factorial( uint32_t number ) {
    return number <= 1 ? number : factorial(number-1) * number;
}

TEST_CASE( "Factorials are computed", "[factorial]" ) {
    REQUIRE( factorial( 1) == 1 );
    REQUIRE( factorial( 2) == 2 );
    REQUIRE( factorial( 3) == 6 );
    REQUIRE( factorial(10) == 3'628'800 );
}

TEST_CASE( "Parse a JSON request", "[request]" ) {
    std::string_view json_raw = R"(
        \"id\": \"fluxo-test-req-001\",\\
        \"imp\": [ \\
            { \\
            \"id\": \"1\", \\
            \"banner\": { \\
                \"w\": 320, \\
                \"h\": 50, \\
                \"pos\": 1 \\
            },
            \"bidfloor\": 0.50, \\
            \"bidfloorcur\": \"USD\" \\
            } \\
        ], \\
        \"site\": { \\
            \"id\": \"site-123\", \\
            \"domain\": \"example-news.com\", \\
            \"page\": \"http://example-news.com\" \\
        }, \\
        \"device\": { \\
            \"ua\": \"Mozilla/5.0 (iPhone; CPU iPhone OS 15_0 like Mac OS X)\", \\
            \"ip\": \"192.168.1.1\", \\
            \"os\": \"iOS\", \\
            \"devicetype\": 4 \\
        }, \\
        \"user\": { \\
            \"id\": \"user-456\" \\
        }, \\
        \"tmax\": 120, \\
        \"at\": 2 \\
    })";

    fluxobid::Bidder bidder;
    bidder.parse_request(json_raw);

    /* DO THIS */
}


TEST_CASE("Bidder parses OpenRTB 2.5 JSON correctly test 1", "[openrtb][parsing]") {
    fluxobid::Bidder bidder;

    std::string_view json_raw1 = R"({
      "id": "fluxo-test-req-001",
      "imp": [{
          "id": "1",
          "banner": { "w": 320, "h": 50 },
          "bidfloor": 0.50
      }],
      "tmax": 120
    })";

    SECTION("Fields are mapped accurately #1") {
        bidder.parse_request(json_raw1);
        auto& req = bidder.get_current_request();
        std::cout << "here\n";

        // Validate top-level ID
        //REQUIRE(req.id == "fluxo-test-req-001");

        // Validate Impression array
        REQUIRE(req.imps.size() == 1);
        REQUIRE(req.imps[0].id == "1");
        
        // Validate Bid Floor (using REQUIRE_THAT for floating point if needed)
        REQUIRE(req.imps[0].bidfloor.has_value());
        REQUIRE(req.imps[0].bidfloor.value() == 0.50);
    }

    SECTION("Handles malformed JSON gracefully 1") {
        std::string_view broken_json = "{ \"id\": \"bad-req\", \"imp\": [ "; 
        REQUIRE(bidder.parse_request(broken_json) == false);
    }
}

TEST_CASE("Bidder parses OpenRTB 2.5 JSON correctly test 2", "[openrtb][parsing]") {
    fluxobid::Bidder bidder;

    std::string_view json_raw2 = R"({
        "id": "fluxo-test-req-002",
        "imp": [{
            "id": "2",
            "banner": { "w": 410, "h": 60 },
            "bidfloor": 0.60
        },
        {
            "id": "3",
            "banner": { "w": 330, "h": 57 },
            "bidfloor": 0.44
        }],
        "tmax": 130
    })";

    SECTION("Fields are mapped accurately #2") {
        bidder.parse_request(json_raw2);
        auto& req = bidder.get_current_request();

        // Validate top-level ID
        REQUIRE(req.id == "fluxo-test-req-002");

        // Validate Impression array
        REQUIRE(req.imps.size() == 2);
        REQUIRE(req.imps[0].id == "2");
        REQUIRE(req.imps[1].id == "3");
        
        // Validate Bid Floor (using REQUIRE_THAT for floating point if needed)
        REQUIRE(req.imps[0].bidfloor.has_value());
        REQUIRE(req.imps[0].bidfloor.value() == 0.60);
    }

    SECTION("Handles malformed JSON gracefully 2") {
        std::string_view broken_json = "{ \"id\": \"bad-req\", \"imp\": [ "; 
        REQUIRE(bidder.parse_request(broken_json) == false);
    }
}

TEST_CASE("Bidder parses OpenRTB 2.5 JSON correctly test 3", "[openrtb][parsing]") {
    fluxobid::Bidder bidder;

    std::string_view json_raw = R"({
        "id": "fluxo-test-req-003",
        "imp": [{
            "id": "4",
            "banner": { "w": 230, "h": 57 },
            "bidfloor": 0.44
        },
        {
            "id": "5",
            "banner": { "w": 270, "h": 50 },
            "bidfloor": 0.44
        },
        {
            "id": "6",
            "banner": { "w": 220, "h": 52 },
            "bidfloor": 0.40
        }],
        "tmax": 117
    })";
    
    SECTION("Successful parsing returns true #3") {
        REQUIRE(bidder.parse_request(json_raw) == true);
        auto& req = bidder.get_current_request();

        REQUIRE(req.imps.size() == 3);
        REQUIRE(req.imps[0].id == "4");
        REQUIRE(req.imps[1].id == "5");
        REQUIRE(req.imps[2].id == "6");

        REQUIRE(req.imps[0].bidfloor.has_value());
        REQUIRE(req.imps[0].bidfloor.value() == 0.44);
    }

    SECTION("Handles malformed JSON gracefully 3") {
        std::string_view broken_json = "{ \"id\": \"bad-req\", \"imp\": [ "; 
        REQUIRE(bidder.parse_request(broken_json) == false);
    }
}
