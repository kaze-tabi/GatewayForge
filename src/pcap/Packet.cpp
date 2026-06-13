#include "gatewayforge/pcap/Packet.h"
#include <cstring>

namespace gatewayforge {

Packet::Packet()
    : timestamp_us_(0)
    , src_port_(0)
    , dst_port_(0)
    , protocol_(TransportProtocol::UNKNOWN)
    , tcp_seq_(0)
    , tcp_ack_(0)
    , tcp_flags_(0)
{
}

Packet::Packet(const Packet& other)
    : timestamp_us_(other.timestamp_us_)
    , src_ip_(other.src_ip_)
    , dst_ip_(other.dst_ip_)
    , src_port_(other.src_port_)
    , dst_port_(other.dst_port_)
    , protocol_(other.protocol_)
    , tcp_seq_(other.tcp_seq_)
    , tcp_ack_(other.tcp_ack_)
    , tcp_flags_(other.tcp_flags_)
{
    if (other.payload_) {
        payload_ = std::make_unique<Buffer>(other.payload_->Data(), other.payload_->Size());
    }
}

Packet& Packet::operator=(const Packet& other) {
    if (this != &other) {
        timestamp_us_ = other.timestamp_us_;
        src_ip_ = other.src_ip_;
        dst_ip_ = other.dst_ip_;
        src_port_ = other.src_port_;
        dst_port_ = other.dst_port_;
        protocol_ = other.protocol_;
        tcp_seq_ = other.tcp_seq_;
        tcp_ack_ = other.tcp_ack_;
        tcp_flags_ = other.tcp_flags_;
        if (other.payload_) {
            payload_ = std::make_unique<Buffer>(other.payload_->Data(), other.payload_->Size());
        } else {
            payload_.reset();
        }
    }
    return *this;
}

void Packet::SetPayload(const uint8_t* data, size_t len) {
    if (data && len > 0) {
        payload_ = std::make_unique<Buffer>(data, len);
    } else {
        payload_ = std::make_unique<Buffer>(0);
    }
}

} // namespace gatewayforge
