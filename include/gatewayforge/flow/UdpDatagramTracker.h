#pragma once

#include <cstdint>
#include <vector>
#include "gatewayforge/pcap/Packet.h"
#include "gatewayforge/flow/../protocol/DecodedMessage.h"

namespace gatewayforge {

class UdpDatagramTracker {
public:
    void AddDatagram(const Packet& packet);
    const std::vector<Packet>& Datagrams() const { return datagrams_; }
    size_t Count() const { return datagrams_.size(); }
    size_t ByteCount() const;

    // Analysis
    size_t MissingSeqCount() const { return missing_seq_count_; }
    size_t OutOfOrderCount() const { return out_of_order_count_; }
    size_t DuplicateCount() const { return duplicate_count_; }
    double LossRate() const;
    double ReorderRate() const;

private:
    std::vector<Packet> datagrams_;
    size_t missing_seq_count_;
    size_t out_of_order_count_;
    size_t duplicate_count_;
};

} // namespace gatewayforge
