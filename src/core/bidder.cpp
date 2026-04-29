#include <asio.hpp>
#include <yyjson.h>

#include "../../include/bidder.hpp"
#include "../../include/metrics.hpp"
#include "../../include/engine.hpp"
#include "../../include/geoprovider.hpp"

namespace fluxobid {

void Bidder::send_http_204(asio::ip::tcp::socket& socket) {
    // The most minimal valid HTTP response
    static constexpr std::string_view response = 
        "HTTP/1.1 204 No Content\r\n"
        "Content-Length: 0\r\n"
        "Connection: keep-alive\r\n"
        "\r\n";
    
    std::cout << "Sending response...\n";
    asio::write(socket, asio::buffer(response));
}

void Bidder::send_http_200(asio::ip::tcp::socket& socket, const std::vector<Bid>& bids) {
    yyjson_mut_doc* doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val* root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);

    yyjson_mut_obj_add_str(doc, root, "id", current_request_.id.data());
    yyjson_mut_obj_add_str(doc, root, "cur", "USD");

    yyjson_mut_val* seatbid_arr = yyjson_mut_obj_add_arr(doc, root, "seatbid");
    yyjson_mut_val* seatbid_obj = yyjson_mut_arr_add_obj(doc, seatbid_arr);
    yyjson_mut_val* bid_arr = yyjson_mut_obj_add_arr(doc, seatbid_obj, "bid");

    for (const auto& b : bids) {
        yyjson_mut_val* bid_obj = yyjson_mut_arr_add_obj(doc, bid_arr);
        yyjson_mut_obj_add_str(doc, bid_obj, "impid", b.imp_id.data());
        yyjson_mut_obj_add_real(doc, bid_obj, "price", b.price);
        yyjson_mut_obj_add_str(doc, bid_obj, "adid", b.ad_id.data());
    }

    size_t len;
    char* json_body = yyjson_mut_write(doc, 0, &len);

    std::string http_response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: " + std::to_string(len) + "\r\n"
        "Connection: keep-alive\r\n"
        "\r\n" + 
        std::string(json_body, len);

    std::cout << "Sending response...\n";
    asio::write(socket, asio::buffer(http_response));
    
    free(json_body);
    yyjson_mut_doc_free(doc);
}



Bidder::Bidder(const CampaignStore& store) : store_(store) { }

bool Bidder::parse_request(std::string_view json_raw) {
    yyjson_doc* doc = yyjson_read(json_raw.data(), json_raw.size(), 0);
    if (!doc) return false;

    yyjson_val* root = yyjson_doc_get_root(doc);
    
    yyjson_val* id_val = yyjson_obj_get(root, "id");
    if (id_val) {
        current_request_.id = yyjson_get_str(id_val);
        std::string temp = yyjson_get_str(id_val);
        std::cout << "before: temp: " << temp << "  " << typeid(temp).name() << '\n';
    }
    std::cout << "before: current request ID: " << current_request_.id << "  " << typeid(current_request_.id).name() << '\n';

    yyjson_val* imp_arr = yyjson_obj_get(root, "imp");
    if (yyjson_is_arr(imp_arr)) {
        size_t idx, max;
        yyjson_val* imp_val;
        yyjson_arr_foreach(imp_arr, idx, max, imp_val) {
            Imp imp;
            imp.id = yyjson_get_str(yyjson_obj_get(imp_val, "id"));
            
            auto floor_val = yyjson_obj_get(imp_val, "bidfloor");
            if (floor_val) imp.bidfloor = yyjson_get_num(floor_val);
            
            current_request_.imps.push_back(imp);
        }
    }

    for (long unsigned int i = 0; i < current_request_.imps.size(); i++) {
        std::cout << " current_request_.imps[" << i << "].id: " << current_request_.imps[i].id << "\n";
        if (current_request_.imps[i].bidfloor.has_value()) {
            std::cout << "  current_request_.imps[" << i << "].bidfloor: " << current_request_.imps[i].bidfloor.value() << "\n";
        } else {
            std::cout << "  current_request_.imps[" << i << "].bidfloor: nullopt\n";
        }
    }

    yyjson_doc_free(doc);
    std::cout << "after: current request ID: " << current_request_.id << '\n';
    std::cout << "\nget ID: " <<yyjson_get_str(id_val) << "\n\n\n";

    std::cout << "Parsed BidRequest ID: " << current_request_.id << " with " 
              << current_request_.imps.size() << " impressions." << "\n\n\n";

    return !current_request_.id.empty();
}


const BidRequest& Bidder::get_current_request() const {
    return this->current_request_;
}

RejectReason Bidder::validate_request(const BidRequest& req) {
    if (req.id.empty()) return RejectReason::MALFORMED_JSON;

    if (req.imps.empty()) return RejectReason::NO_IMPRESSIONS;

    if (req.tmax.has_value() && req.tmax.value() < 20) {
        return RejectReason::TIMEOUT_TOO_LOW;
    }

    return RejectReason::NONE;
}

void Bidder::handle_request(asio::ip::tcp::socket& socket, std::string_view json_raw) {
    if (!parse_request(json_raw)) {
        send_http_204(socket);
        return;
    }

    if (current_request_.imps.empty() || current_request_.id.empty()) {
        send_http_204(socket);
        return;
    }

    Engine engine;
    static GeoProvider geo; 
    std::string_view country = geo.lookup(current_request_.device_ip);
    auto bids = engine.evaluate_request(current_request_, store_, country);
    std::cout << "Bids found: " << bids.size() << "\n";

    if (bids.empty()) {
        send_http_204(socket);
    } else {
        send_http_200(socket, bids);
    }
}

}
