#pragma once
#include <string_view>
#include <vector>
#include <optional>

namespace fluxobid {

// --- Sub-Objects ---
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

// --- Top-Level BidRequest Object ---
struct BidRequest {
    std::string_view id;
    std::vector<Imp> imps;
    std::optional<Site> site;
    std::optional<Device> device;
    std::string_view device_ip;
    std::optional<int> tmax;
};

class Bidder {
public:
    bool parse_request(std::string_view json_raw);
    const BidRequest& get_current_request() const;
   
private:
    BidRequest current_request_;
};

} // namespace fluxobid
