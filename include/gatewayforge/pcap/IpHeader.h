#pragma once

#include <cstdint>

namespace gatewayforge {

struct Ipv4Header {
    uint8_t version_ihl;
    uint8_t dscp_ecn;
    uint16_t total_length;
    uint16_t identification;
    uint16_t flags_fragment;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t src_ip;
    uint32_t dst_ip;

    static constexpr uint8_t PROTO_TCP = 6;
    static constexpr uint8_t PROTO_UDP = 17;
};

} // namespace gatewayforge
