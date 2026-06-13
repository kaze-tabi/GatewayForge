#pragma once

#include <cstdint>
#include <chrono>
#include <string>

namespace gatewayforge {

class Time {
public:
    static uint64_t NowNs();
    static uint64_t NowUs();
    static uint64_t NowMs();

    static std::string FormatNs(uint64_t ns);
    static std::string FormatUs(uint64_t us);
    static std::string FormatMs(uint64_t ms);
};

} // namespace gatewayforge
