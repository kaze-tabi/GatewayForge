#pragma once

#include <cstdint>
#include <string>

namespace gatewayforge {

struct Message {
    uint64_t timestamp_us;
    std::string direction;
    std::string type;
    std::string summary;

    Message() : timestamp_us(0) {}
    Message(uint64_t ts, std::string dir, std::string t, std::string sum)
        : timestamp_us(ts), direction(std::move(dir)), type(std::move(t)), summary(std::move(sum)) {}
};

} // namespace gatewayforge
