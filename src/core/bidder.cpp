#ifndef __INTELLISENSE__
module;
#endif

#include <bits/c++config.h>
#include <asio.hpp>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include "../../include/bidder.hpp"
#include <yyjson.h>

#ifndef __INTELLISENSE__
export module fluxobid.bidder;
#endif

namespace fluxobid {

bool Bidder::parse_request(std::string_view json_raw) {
    yyjson_doc* doc = yyjson_read(json_raw.data(), json_raw.size(), 0);
    if (!doc) return false;

    yyjson_val* root = yyjson_doc_get_root(doc);
    
    yyjson_val* id_val = yyjson_obj_get(root, "id");
    if (id_val) {
        current_request_.id = yyjson_get_str(id_val);
        std::string temp = yyjson_get_str(id_val);
        std::cout << "\nbefore: temp: " << temp << "  " << typeid(temp).name() << '\n';
    }
    std::cout << "\nbefore: current request ID: " << current_request_.id << "  " << typeid(current_request_.id).name() << '\n';

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

    for (int i = 0; i < current_request_.imps.size(); ++i) {
        std::cout << "current_request_.imps[" << i << "].id: " << current_request_.imps[i].id << "\n";
        if (current_request_.imps[i].bidfloor.has_value()) {
            std::cout << "current_request_.imps[" << i << "].bidfloor: " << current_request_.imps[i].bidfloor.value() << "\n";
        } else {
            std::cout << "current_request_.imps[" << i << "].bidfloor: nullopt\n";
        }
    }

    yyjson_doc_free(doc);
    
    std::cout << "Parsed BidRequest ID: " << current_request_.id << " with " 
              << current_request_.imps.size() << " impressions." << std::endl;

    return !current_request_.id.empty();
}


const BidRequest& Bidder::get_current_request() const {
    return this->current_request_;
}

}
