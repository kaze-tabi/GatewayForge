#include "gatewayforge/flow/TcpFlow.h"

namespace gatewayforge {

TcpFlow::TcpFlow(const FlowKey& key)
    : key_(key)
    , packet_count_(0)
    , first_seen_(0)
    , last_seen_(0)
{
}

void TcpFlow::OnPacket(const Packet& packet) {
    if (packet_count_ == 0) {
        first_seen_ = packet.TimestampUs();
    }
    last_seen_ = packet.TimestampUs();
    packet_count_++;

    if (is_client_side(packet)) {
        client_to_server_.AddSegment(packet, true);
    } else {
        server_to_client_.AddSegment(packet, false);
    }
}

std::vector<Message> TcpFlow::Messages() const {
    std::vector<Message> result;
    for (const auto& msg : client_to_server_.Messages()) {
        result.push_back(msg);
    }
    for (const auto& msg : server_to_client_.Messages()) {
        result.push_back(msg);
    }
    return result;
}

bool TcpFlow::is_client_side(const Packet& packet) const {
    return packet.SrcIp() == key_.src_ip && packet.SrcPort() == key_.src_port;
}

} // namespace gatewayforge
