#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include "gatewayforge/common/Buffer.h"

namespace gatewayforge {

enum class TransportProtocol {
    TCP,
    UDP,
    UNKNOWN
};

class Packet {
public:
    Packet();
    ~Packet() = default;

    // Copy
    Packet(const Packet& other);
    Packet& operator=(const Packet& other);

    // Move
    Packet(Packet&& other) noexcept = default;
    Packet& operator=(Packet&& other) noexcept = default;

    void SetTimestamp(uint64_t ts_us) { timestamp_us_ = ts_us; }
    uint64_t TimestampUs() const { return timestamp_us_; }

    void SetSrcIp(const std::string& ip) { src_ip_ = ip; }
    const std::string& SrcIp() const { return src_ip_; }

    void SetDstIp(const std::string& ip) { dst_ip_ = ip; }
    const std::string& DstIp() const { return dst_ip_; }

    void SetSrcPort(uint16_t port) { src_port_ = port; }
    uint16_t SrcPort() const { return src_port_; }

    void SetDstPort(uint16_t port) { dst_port_ = port; }
    uint16_t DstPort() const { return dst_port_; }

    void SetProtocol(TransportProtocol proto) { protocol_ = proto; }
    TransportProtocol Protocol() const { return protocol_; }

    void SetPayload(const uint8_t* data, size_t len);
    const Buffer& Payload() const { return *payload_; }
    Buffer& Payload() { return *payload_; }
    size_t PayloadSize() const { return payload_->Size(); }

    void SetTcpSeq(uint32_t seq) { tcp_seq_ = seq; }
    uint32_t TcpSeq() const { return tcp_seq_; }

    void SetTcpAck(uint32_t ack) { tcp_ack_ = ack; }
    uint32_t TcpAck() const { return tcp_ack_; }

    void SetTcpFlags(uint8_t flags) { tcp_flags_ = flags; }
    uint8_t TcpFlags() const { return tcp_flags_; }

    bool IsSyn() const { return tcp_flags_ & 0x02; }
    bool IsFin() const { return tcp_flags_ & 0x01; }
    bool IsRst() const { return tcp_flags_ & 0x04; }
    bool IsAck() const { return tcp_flags_ & 0x10; }

private:
    uint64_t timestamp_us_;
    std::string src_ip_;
    std::string dst_ip_;
    uint16_t src_port_;
    uint16_t dst_port_;
    TransportProtocol protocol_;
    std::unique_ptr<Buffer> payload_;
    uint32_t tcp_seq_;
    uint32_t tcp_ack_;
    uint8_t tcp_flags_;
};

} // namespace gatewayforge
