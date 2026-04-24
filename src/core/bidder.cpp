/*#ifndef __INTELLISENSE__
module;
#endif*/

/*#include <sys/types.h>
#include <endian.h>
#include <byteswap.h>

#define ASIO_NO_TS_EXECUTORS
#define ASIO_DISABLE_CONCEPTS
#define ASIO_HAS_STD_COROUTINE 1*/

#include <asio.hpp>
#include "../../include/bidder.hpp"
#include "../../include/metrics.hpp"
#include "../../include/engine.hpp"
#include <yyjson.h>

/*#ifndef __INTELLISENSE__
export module fluxobid.bidder;
#endif*/

namespace fluxobid {

bool Bidder::parse_request(std::string_view json_raw) {
    // Read the JSON (Zero-copy flag)
    yyjson_doc* doc = yyjson_read(json_raw.data(), json_raw.size(), 0);
    if (!doc) return false;

    yyjson_val* root = yyjson_doc_get_root(doc);
    
    // Map OpenRTB fields to our Struct
    // "id" is required in OpenRTB 2.5
    yyjson_val* id_val = yyjson_obj_get(root, "id");
    if (id_val) {
        _current_request.id = yyjson_get_str(id_val);
        std::string temp = yyjson_get_str(id_val);
        std::cout << "before: temp: " << temp << "  " << typeid(temp).name() << '\n';
    }
    //auto temp = (std::string) _current_request.id;
    std::cout << "before: current request ID: " << _current_request.id << "  " << typeid(_current_request.id).name() << '\n';

    // Dig into the Impression array
    yyjson_val* imp_arr = yyjson_obj_get(root, "imp");
    if (yyjson_is_arr(imp_arr)) {
        size_t idx, max;
        yyjson_val* imp_val;
        yyjson_arr_foreach(imp_arr, idx, max, imp_val) {
            Imp imp;
            imp.id = yyjson_get_str(yyjson_obj_get(imp_val, "id"));
            
            auto floor_val = yyjson_obj_get(imp_val, "bidfloor");
            if (floor_val) imp.bidfloor = yyjson_get_num(floor_val);
            
            _current_request.imps.push_back(imp);
        }
    }

    for (long unsigned int i = 0; i < _current_request.imps.size(); i++) {
        std::cout << " _current_request.imps[" << i << "].id: " << _current_request.imps[i].id << "\n";
        if (_current_request.imps[i].bidfloor.has_value()) {
            std::cout << "  _current_request.imps[" << i << "].bidfloor: " << _current_request.imps[i].bidfloor.value() << "\n";
        } else {
            std::cout << "  _current_request.imps[" << i << "].bidfloor: nullopt\n";
        }
    }

    // Clean up the parser (not the strings, they point to json_raw!)
    //_current_request.id = temp;
    yyjson_doc_free(doc);
    std::cout << "after: current request ID: " << _current_request.id << '\n';
    //std::cout << "\nafter: temp: " << temp << '\n';
    std::cout << "\nget ID: " <<yyjson_get_str(id_val) << "\n\n\n";

    std::cout << "Parsed BidRequest ID: " << _current_request.id << " with " 
              << _current_request.imps.size() << " impressions." << "\n\n\n";

    return !_current_request.id.empty();
}


const BidRequest& Bidder::get_current_request() const {
    return this->_current_request;
}

RejectReason Bidder::validate_request(const BidRequest& req) {
    // Mandatory ID check
    if (req.id.empty()) return RejectReason::MALFORMED_JSON;

    // Empty Impression check
    if (req.imps.empty()) return RejectReason::NO_IMPRESSIONS;

    // Deadline check (Crucial for high-concurrency)
    // If tmax is less than 20ms, we likely won't finish in time after network jitter is factored in.
    if (req.tmax.has_value() && req.tmax.value() < 20) {
        return RejectReason::TIMEOUT_TOO_LOW;
    }

    return RejectReason::NONE;
}

void Bidder::handle_request(std::string_view json_raw) {
    // A. Parse
    if (!parse_request(json_raw)) {
        asio::io_context io;
        asio::ip::tcp::socket socket(io);

        send_http_204(socket); // Malformed JSON
        return;
    }

    Metrics metrics;
    // B. Early Exit Check
    auto reason = validate_request(_current_request);
    if (reason != RejectReason::NONE) {
        // Increment a metric so you know WHY you are skipping bids
        metrics.record_skip(reason); 

        asio::io_context io;
        asio::ip::tcp::socket socket(io);
        
        send_http_204(socket); // Fast exit
        return;
    }

    Engine engine;
    // C. Only if valid, proceed to expensive Engine logic
    auto bids = engine.evaluate_request(_current_request, this->_store);
    
    if (bids.empty()) {
        asio::io_context io;
        asio::ip::tcp::socket socket(io);
        
        send_http_204(socket); // No campaigns matched
    } else {
        asio::io_context io;
        asio::ip::tcp::socket socket(io);

        send_bid_response(socket, bids);
    }
}


void Bidder::send_http_204(asio::ip::tcp::socket& socket) {
    // The most minimal valid HTTP response
    static constexpr std::string_view response = 
        "HTTP/1.1 204 No Content\r\n"
        "Content-Length: 0\r\n"
        "Connection: keep-alive\r\n"
        "\r\n";
    
    asio::write(socket, asio::buffer(response));
}


void Bidder::send_bid_response(asio::ip::tcp::socket& socket, const std::vector<Bid>& bids) {
    // Create a new JSON doc for writing
    yyjson_mut_doc* doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val* root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);

    // Set Top-Level Fields
    // 'id' must match the incoming BidRequest.id
    yyjson_mut_obj_add_str(doc, root, "id", _current_request.id.data());
    yyjson_mut_obj_add_str(doc, root, "cur", "USD");

    // Build the SeatBid array (required by spec)
    yyjson_mut_val* seatbid_arr = yyjson_mut_obj_add_arr(doc, root, "seatbid");
    yyjson_mut_val* seatbid_obj = yyjson_mut_arr_add_obj(doc, seatbid_arr);
    yyjson_mut_val* bid_arr = yyjson_mut_obj_add_arr(doc, seatbid_obj, "bid");

    for (const auto& b : bids) {
        yyjson_mut_val* bid_obj = yyjson_mut_arr_add_obj(doc, bid_arr);
        yyjson_mut_obj_add_str(doc, bid_obj, "impid", b._imp_id.data());
        yyjson_mut_obj_add_real(doc, bid_obj, "price", b._price);
        yyjson_mut_obj_add_str(doc, bid_obj, "adid", b._ad_id.data());
        // In a real app, you'd add "adm" (ad markup) here
    }

    // Serialize to string
    size_t len;
    char* json_body = yyjson_mut_write(doc, 0, &len);

    // Construct HTTP 200 Header + Body
    std::string http_response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: " + std::to_string(len) + "\r\n"
        "Connection: keep-alive\r\n"
        "\r\n" + 
        std::string(json_body, len);

    // Send and Cleanup
    asio::write(socket, asio::buffer(http_response));
    
    free(json_body); // Free the string allocated by yyjson
    yyjson_mut_doc_free(doc);
}

}
