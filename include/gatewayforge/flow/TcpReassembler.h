#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include "gatewayforge/flow/FlowKey.h"
#include "gatewayforge/pcap/Packet.h"
#include "gatewayforge/flow/../protocol/DecodedMessage.h"

namespace gatewayforge {

class TcpReassembler {
public:
    TcpReassembler();

    void AddSegment(const Packet& packet, bool is_client_to_server);
    std::vector<Message> Messages() const;
    std::vector<uint8_t> Stream() const { return stream_; }
    size_t ByteCount() const { return stream_.size(); }
    uint32_t NextSeq() const { return next_seq_; }

private:
    std::vector<uint8_t> stream_;
    uint32_t next_seq_;
    bool initialized_;
};

} // namespace gatewayforge
