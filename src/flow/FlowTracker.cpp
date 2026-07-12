#include "gatewayforge/flow/FlowTracker.h"
#include "gatewayforge/flow/TcpFlow.h"
#include "gatewayforge/flow/UdpFlow.h"
#include <memory>

namespace gatewayforge {

FlowKey FlowTracker::MakeKey(const Packet& packet) const {
    FlowKey key;
    key.src_ip = packet.SrcIp();
    key.src_port = packet.SrcPort();
    key.dst_ip = packet.DstIp();
    key.dst_port = packet.DstPort();
    key.protocol = packet.Protocol();
    return key.Normalized();
}

void FlowTracker::OnPacket(const Packet& packet) {
    auto key = MakeKey(packet);
    auto it = flows_.find(key);
    if (it == flows_.end()) {
        if (packet.Protocol() == TransportProtocol::TCP) {
            flows_[key] = std::make_unique<TcpFlow>(key);
        } else if (packet.Protocol() == TransportProtocol::UDP) {
            flows_[key] = std::make_unique<UdpFlow>(key);
        }
    }
    flows_[key]->OnPacket(packet);
}

std::vector<const Flow*> FlowTracker::GetFlows() const {
    std::vector<const Flow*> result;
    for (const auto& pair : flows_) {
        result.push_back(pair.second.get());
    }
    return result;
}

size_t FlowTracker::TcpFlowCount() const {
    size_t count = 0;
    for (const auto& pair : flows_) {
        if (pair.second->Protocol() == TransportProtocol::TCP) count++;
    }
    return count;
}

size_t FlowTracker::UdpFlowCount() const {
    size_t count = 0;
    for (const auto& pair : flows_) {
        if (pair.second->Protocol() == TransportProtocol::UDP) count++;
    }
    return count;
}

} // namespace gatewayforge
