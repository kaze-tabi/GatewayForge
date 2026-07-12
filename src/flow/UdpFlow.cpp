#include "gatewayforge/flow/UdpFlow.h"
#include <sstream>

namespace gatewayforge {

UdpFlow::UdpFlow(const FlowKey& key)
    : key_(key)
    , first_seen_(0)
    , last_seen_(0)
{
}

void UdpFlow::OnPacket(const Packet& packet) {
    if (tracker_.Count() == 0) {
        first_seen_ = packet.TimestampUs();
    }
    last_seen_ = packet.TimestampUs();
    tracker_.AddDatagram(packet);
}

std::vector<Message> UdpFlow::Messages() const {
    std::vector<Message> result;
    for (size_t i = 0; i < tracker_.Count(); i++) {
        const auto& d = tracker_.Datagrams()[i];
        Message msg;
        msg.timestamp_us = d.TimestampUs();
        msg.type = "UDP_DATAGRAM";
        msg.direction = d.SrcIp() + ":" + std::to_string(d.SrcPort()) +
                        " -> " + d.DstIp() + ":" + std::to_string(d.DstPort());
        std::ostringstream oss;
        oss << "datagram #" << (i + 1) << " (" << d.PayloadSize() << " bytes)";
        msg.summary = oss.str();
        result.push_back(std::move(msg));
    }
    return result;
}

} // namespace gatewayforge
