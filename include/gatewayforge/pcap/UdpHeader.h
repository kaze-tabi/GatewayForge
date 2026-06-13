#pragma once

#include <cstdint>

namespace gatewayforge {

struct UdpHeader {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
};

} // namespace gatewayforge
