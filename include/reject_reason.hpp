#pragma once

enum class RejectReason {
    NONE,
    MALFORMED_JSON,
    NO_IMPRESSIONS,
    TIMEOUT_TOO_LOW,
    PRIVACY_RESTRICTED
};