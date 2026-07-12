#pragma once

#include <string>
#include <cstdint>

namespace gatewayforge {

struct ReplayOptions {
    std::string pcap_path;
    std::string target_host;
    uint16_t target_port;
    std::string transport; // "tcp" or "udp"
    double speed;
    bool randomize_source_port;

    ReplayOptions()
        : target_port(0), speed(1.0), randomize_source_port(false) {}
};

} // namespace gatewayforge
