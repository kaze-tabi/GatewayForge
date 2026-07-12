#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include "gatewayforge/flow/FlowKey.h"
#include "gatewayforge/pcap/Packet.h"

namespace gatewayforge {

class Flow {
public:
    virtual ~Flow() = default;

    virtual TransportProtocol Protocol() const = 0;
    virtual void OnPacket(const Packet& packet) = 0;
    virtual FlowKey Key() const = 0;
    virtual size_t PacketCount() const = 0;
    virtual size_t ByteCount() const = 0;
    virtual uint64_t FirstSeenUs() const = 0;
    virtual uint64_t LastSeenUs() const = 0;
};

class FlowTracker {
public:
    void OnPacket(const Packet& packet);
    const std::unordered_map<FlowKey, std::unique_ptr<Flow>>& Flows() const { return flows_; }
    std::vector<const Flow*> GetFlows() const;
    size_t FlowCount() const { return flows_.size(); }
    size_t TcpFlowCount() const;
    size_t UdpFlowCount() const;

private:
    std::unordered_map<FlowKey, std::unique_ptr<Flow>> flows_;
    FlowKey MakeKey(const Packet& packet) const;
};

} // namespace gatewayforge
