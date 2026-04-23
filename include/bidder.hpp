#pragma once
#include <string_view>
#include <vector>
#include <optional>

namespace fluxobid {

// --- Sub-Objects ---
struct Banner {
    std::optional<int> w;            // Width in pixels
    std::optional<int> h;            // Height in pixels
    std::vector<int> battr;          // Blocked creative attributes
    std::optional<int> pos;          // Ad position (e.g., above the fold)
};

struct Imp {
    std::string_view id;             // Unique ID for this impression
    std::optional<Banner> banner;    // Banner object (optional if Video/Native)
    std::optional<double> bidfloor;  // Minimum bid price (CPM)
    std::string_view bidfloorcur = "USD"; 
};

struct Device {
    std::string_view ua;             // User agent
    std::string_view ip;             // IPv4 or IPv6 address
    std::optional<int> devicetype;   // 4=Phone, 5=Tablet, etc.
    std::string_view os;             // Device OS (e.g., "iOS", "Android")
};

struct Site {
    std::string_view id;             // Site ID
    std::string_view page;           // URL of the page
    std::optional<std::string_view> domain;
};

// --- Top-Level BidRequest Object ---
struct BidRequest {
    std::string id;             // Unique request ID [REQUIRED]
    std::vector<Imp> imps;           // Array of impressions [REQUIRED]
    std::optional<Site> site;        // Site object (mutually exclusive with App)
    std::optional<Device> device;    // Device information
    std::string_view device_ip;
    std::optional<int> tmax;         // Max time for bid in ms
};

class Bidder {
public:
    // Takes raw JSON, returns true if we want to bid
    bool parse_request(std::string_view json_raw);
    const BidRequest& get_current_request() const;
    
private:
    BidRequest current_request_;
};

} // namespace fluxobid
