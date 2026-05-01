#pragma once

#include <concepts>
#include <optional>
#include <vector>
#include <string_view>

namespace fluxobid {

struct Banner {
    std::optional<int> w;
    std::optional<int> h;
    std::vector<int> battr;
    std::optional<int> pos;
};

struct Imp {
    std::string_view id;
    std::optional<Banner> banner;
    std::optional<double> bidfloor;
    std::string_view bidfloorcur = "USD"; 

    Imp() = default;
    Imp(const std::string_view id, const std::optional<double> bidfloor) : id(id), bidfloor(bidfloor)  {}
};

struct Device {
    std::string_view ua;
    std::string_view ip;
    std::optional<int> devicetype;
    std::string_view os;
};

struct Site {
    std::string_view id;
    std::string_view page;
    std::optional<std::string_view> domain;
};

struct BidRequest {
    std::string_view id;
    std::vector<Imp> imps;
    std::optional<Site> site;
    std::optional<Device> device;
    std::string_view device_ip;
    std::optional<int> tmax;

    BidRequest() = default;
    BidRequest(const std::string_view id, const std::vector<Imp> imps, const std::string_view device_ip) : id(id), imps(imps), device_ip(device_ip) {}
};

}
