#pragma once

#include <string>
#include <cstdint>
#include "gatewayforge/pcap/Packet.h"

namespace gatewayforge {

struct FlowKey {
    std::string src_ip;
    uint16_t src_port;
    std::string dst_ip;
    uint16_t dst_port;
    TransportProtocol protocol;

    bool operator==(const FlowKey& other) const {
        return src_ip == other.src_ip &&
               src_port == other.src_port &&
               dst_ip == other.dst_ip &&
               dst_port == other.dst_port &&
               protocol == other.protocol;
    }

    FlowKey Normalized() const;
    std::string ToString() const;
};

} // namespace gatewayforge

namespace std {
    template<> struct hash<gatewayforge::FlowKey> {
        size_t operator()(const gatewayforge::FlowKey& k) const;
    };
}
