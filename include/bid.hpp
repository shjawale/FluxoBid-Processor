#pragma once

#include <iostream>
#include <string_view>

namespace fluxobid {

struct Bid {
    std::string_view imp_id;
    double price;
    std::string_view ad_id;

    Bid() = default;
    Bid(const std::string_view imp_id, double price, std::string_view ad_id) : imp_id(imp_id), price(price), ad_id(ad_id) { }
};

}