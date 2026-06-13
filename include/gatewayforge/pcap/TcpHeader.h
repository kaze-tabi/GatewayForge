#pragma once

#include <cstdint>

namespace gatewayforge {

struct TcpHeader {
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t seq;
    uint32_t ack;
    uint8_t data_offset_reserved;
    uint8_t flags;
    uint16_t window;
    uint16_t checksum;
    uint16_t urgent_pointer;

    static constexpr uint8_t FLAG_FIN = 0x01;
    static constexpr uint8_t FLAG_SYN = 0x02;
    static constexpr uint8_t FLAG_RST = 0x04;
    static constexpr uint8_t FLAG_PSH = 0x08;
    static constexpr uint8_t FLAG_ACK = 0x10;
    static constexpr uint8_t FLAG_URG = 0x20;
};

} // namespace gatewayforge
