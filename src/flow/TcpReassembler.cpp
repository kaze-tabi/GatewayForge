#include "gatewayforge/flow/TcpReassembler.h"

namespace gatewayforge {

TcpReassembler::TcpReassembler() : next_seq_(0), initialized_(false) {}

void TcpReassembler::AddSegment(const Packet& packet, bool /*is_client_to_server*/) {
    // MVP: simple ordered stream reassembly
    if (!initialized_) {
        next_seq_ = packet.TcpSeq();
        initialized_ = true;
    }

    const uint8_t* data = packet.Payload().Data();
    size_t len = packet.Payload().Size();

    if (len == 0) return;

    uint32_t seg_seq = packet.TcpSeq();

    if (seg_seq == next_seq_) {
        // In-order: append directly
        stream_.insert(stream_.end(), data, data + len);
        next_seq_ = seg_seq + len;
    }
    // MVP does not handle out-of-order or retransmission
}

std::vector<Message> TcpReassembler::Messages() const {
    // Return a single message representing the entire stream
    std::vector<Message> result;
    if (!stream_.empty()) {
        Message msg;
        msg.timestamp_us = 0;
        msg.type = "TCP_STREAM";
        msg.direction = "->";
        msg.summary = "TCP stream (" + std::to_string(stream_.size()) + " bytes)";
        result.push_back(std::move(msg));
    }
    return result;
}

} // namespace gatewayforge
