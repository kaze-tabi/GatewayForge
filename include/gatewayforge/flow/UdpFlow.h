#pragma once

#include <cstdint>
#include <vector>
#include "gatewayforge/flow/FlowKey.h"
#include "gatewayforge/flow/FlowTracker.h"
#include "gatewayforge/flow/UdpDatagramTracker.h"
#include "gatewayforge/pcap/Packet.h"

namespace gatewayforge {

class UdpFlow : public Flow {
public:
    explicit UdpFlow(const FlowKey& key);

    TransportProtocol Protocol() const override { return TransportProtocol::UDP; }
    void OnPacket(const Packet& packet) override;
    FlowKey Key() const override { return key_; }
    size_t PacketCount() const override { return tracker_.Count(); }
    size_t ByteCount() const override { return tracker_.ByteCount(); }
    uint64_t FirstSeenUs() const override { return first_seen_; }
    uint64_t LastSeenUs() const override { return last_seen_; }

    const UdpDatagramTracker& Tracker() const { return tracker_; }
    std::vector<Message> Messages() const;

private:
    FlowKey key_;
    UdpDatagramTracker tracker_;
    uint64_t first_seen_;
    uint64_t last_seen_;
};

} // namespace gatewayforge
