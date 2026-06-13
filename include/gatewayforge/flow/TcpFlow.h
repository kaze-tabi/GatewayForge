#pragma once

#include <vector>
#include <memory>
#include "gatewayforge/flow/FlowKey.h"
#include "gatewayforge/flow/FlowTracker.h"
#include "gatewayforge/flow/TcpReassembler.h"
#include "gatewayforge/pcap/Packet.h"

namespace gatewayforge {

class TcpFlow : public Flow {
public:
    explicit TcpFlow(const FlowKey& key);

    TransportProtocol Protocol() const override { return TransportProtocol::TCP; }
    void OnPacket(const Packet& packet) override;
    FlowKey Key() const override { return key_; }
    size_t PacketCount() const override { return packet_count_; }
    size_t ByteCount() const override { return client_to_server_.ByteCount() + server_to_client_.ByteCount(); }
    uint64_t FirstSeenUs() const override { return first_seen_; }
    uint64_t LastSeenUs() const override { return last_seen_; }

    std::vector<Message> Messages() const;
    const TcpReassembler& ClientToServer() const { return client_to_server_; }
    const TcpReassembler& ServerToClient() const { return server_to_client_; }

private:
    FlowKey key_;
    TcpReassembler client_to_server_;
    TcpReassembler server_to_client_;
    size_t packet_count_;
    uint64_t first_seen_;
    uint64_t last_seen_;
    bool is_client_side(const Packet& packet) const;
};

} // namespace gatewayforge
