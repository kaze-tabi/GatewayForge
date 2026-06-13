#pragma once

#include <memory>
#include <string>
#include "gatewayforge/pcap/Packet.h"
#include "gatewayforge/pcap/EthernetHeader.h"
#include "gatewayforge/pcap/IpHeader.h"
#include "gatewayforge/pcap/TcpHeader.h"
#include "gatewayforge/pcap/UdpHeader.h"

namespace gatewayforge {

class PacketParser {
public:
    static std::unique_ptr<Packet> Parse(const uint8_t* data, size_t len);

private:
    static bool ParseEthernet(const uint8_t* data, size_t len, EthernetHeader& eth);
    static bool ParseIpv4(const uint8_t* data, size_t len, Ipv4Header& ip);
    static bool ParseTcp(const uint8_t* data, size_t len, TcpHeader& tcp);
    static bool ParseUdp(const uint8_t* data, size_t len, UdpHeader& udp);
    static std::string IpToString(uint32_t ip);
};

} // namespace gatewayforge
