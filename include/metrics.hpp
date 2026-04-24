#pragma once

#include <atomic>
#include <iostream>

#include "reject_reason.hpp"

namespace fluxobid {

struct Metrics {
    std::atomic<uint64_t> requests_received{0};
    std::atomic<uint64_t> bids_submitted{0};
    std::atomic<uint64_t> num_bids{0};
    std::atomic<uint64_t> parse_errors{0};
    std::vector<RejectReason> reason_log;

    void record_skip(RejectReason reason) {
        num_bids++;
        reason_log.push_back(reason);
        // You could also log specific reason counts here
    }

    void print_summary() {
        std::cout << "--- FluxoBid Stats ---\n"
                  << "Requests: " << requests_received << "\n"
                  << "Bids:     " << bids_submitted << "\n"
                  << "Errors:   " << parse_errors << "\n";
    }
};

}