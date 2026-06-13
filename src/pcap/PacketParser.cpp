#include "gatewayforge/pcap/PacketParser.h"
#include "gatewayforge/common/Logger.h"
#include <arpa/inet.h>
#include <cstring>

namespace gatewayforge {

std::unique_ptr<Packet> PacketParser::Parse(const uint8_t* data, size_t len) {
    if (!data || len == 0) return nullptr;

    size_t offset = 0;

    // Parse Ethernet
    EthernetHeader eth;
    if (!ParseEthernet(data + offset, len - offset, eth)) return nullptr;
    offset += 14; // Ethernet header size

    // Parse IPv4
    Ipv4Header ip;
    if (!ParseIpv4(data + offset, len - offset, ip)) return nullptr;
    offset += (ip.version_ihl & 0x0F) * 4; // IHL * 4

    auto packet = std::make_unique<Packet>();
    packet->SetSrcIp(IpToString(ip.src_ip));
    packet->SetDstIp(IpToString(ip.dst_ip));

    if (ip.protocol == Ipv4Header::PROTO_TCP) {
        TcpHeader tcp;
        if (!ParseTcp(data + offset, len - offset, tcp)) return nullptr;
        offset += ((tcp.data_offset_reserved >> 4) & 0x0F) * 4;

        packet->SetProtocol(TransportProtocol::TCP);
        packet->SetSrcPort(ntohs(tcp.src_port));
        packet->SetDstPort(ntohs(tcp.dst_port));
        packet->SetTcpSeq(ntohl(tcp.seq));
        packet->SetTcpAck(ntohl(tcp.ack));
        packet->SetTcpFlags(tcp.flags);
    } else if (ip.protocol == Ipv4Header::PROTO_UDP) {
        UdpHeader udp;
        if (!ParseUdp(data + offset, len - offset, udp)) return nullptr;
        offset += 8; // UDP header size

        packet->SetProtocol(TransportProtocol::UDP);
        packet->SetSrcPort(ntohs(udp.src_port));
        packet->SetDstPort(ntohs(udp.dst_port));
    } else {
        return nullptr;
    }

    // Set payload
    if (offset < len) {
        packet->SetPayload(data + offset, len - offset);
    }

    return packet;
}

bool PacketParser::ParseEthernet(const uint8_t* data, size_t len, EthernetHeader& eth) {
    if (len < 14) return false;
    std::memcpy(&eth, data, 14);
    return true;
}

bool PacketParser::ParseIpv4(const uint8_t* data, size_t len, Ipv4Header& ip) {
    if (len < 20) return false;
    std::memcpy(&ip, data, 20);
    return (ip.version_ihl >> 4) == 4;
}

bool PacketParser::ParseTcp(const uint8_t* data, size_t len, TcpHeader& tcp) {
    if (len < 20) return false;
    std::memcpy(&tcp, data, 20);
    return true;
}

bool PacketParser::ParseUdp(const uint8_t* data, size_t len, UdpHeader& udp) {
    if (len < 8) return false;
    std::memcpy(&udp, data, 8);
    return true;
}

std::string PacketParser::IpToString(uint32_t ip) {
    struct in_addr addr;
    addr.s_addr = ip;
    return inet_ntoa(addr);
}

} // namespace gatewayforge
