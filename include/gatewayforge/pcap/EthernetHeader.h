#pragma once

#include <cstdint>

namespace gatewayforge {

struct EthernetHeader {
    uint8_t dst_mac[6];
    uint8_t src_mac[6];
    uint16_t ethertype;

    static constexpr uint16_t ETHERTYPE_IPV4 = 0x0800;
    static constexpr uint16_t ETHERTYPE_IPV6 = 0x86DD;
};

} // namespace gatewayforge
