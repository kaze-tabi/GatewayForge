#pragma once

#include <cstdint>
#include <string>
#include "gatewayforge/protocol/DecodedMessage.h"

namespace gatewayforge {

struct TimelineEvent {
    uint64_t timestamp_us;
    std::string transport;
    std::string src;
    std::string dst;
    std::string message_type;
    std::string details;

    static TimelineEvent FromMessage(const Message& msg, const std::string& transport,
                                      const std::string& src, const std::string& dst);
};

} // namespace gatewayforge
