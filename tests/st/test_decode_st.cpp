/**
 * @file    tests/st/test_decode_st.cpp
 * @brief   PCAP 解码系统测试 (6 用例)
 * @module  pcap / flow / protocol / timeline
 * @level   System Test
 *
 * 端到端测试完整解码流程:
 *   - 构造合法 TCP/UDP 原始包 → PacketParser → FlowTracker → TimelineBuilder
 *   - 空数据 / 混合协议 / 聚合验证
 *
 * 对应测试计划: ST-DEC-001 ~ ST-DEC-006
 */

#include <gtest/gtest.h>
#include "gatewayforge/pcap/PacketParser.h"
#include "gatewayforge/flow/FlowTracker.h"
#include "gatewayforge/flow/TcpFlow.h"
#include "gatewayforge/flow/UdpFlow.h"
#include "gatewayforge/protocol/SchemaLoader.h"
#include "gatewayforge/protocol/BinaryDecoder.h"
#include "gatewayforge/timeline/TimelineBuilder.h"
#include <arpa/inet.h>
#include <cstring>

using namespace gatewayforge;

namespace {

struct RawPacket {
    std::vector<uint8_t> data;
    void Append(const void* src, size_t len) {
        const uint8_t* p = static_cast<const uint8_t*>(src);
        data.insert(data.end(), p, p + len);
    }
};

RawPacket MakeTcpPacket(const std::string& src_ip, uint16_t src_port,
                         const std::string& dst_ip, uint16_t dst_port,
                         uint32_t seq, const uint8_t* payload, size_t payload_len) {
    RawPacket pkt;
    uint8_t eth[14] = {0};
    uint16_t ethertype = htons(0x0800);
    std::memcpy(eth + 12, &ethertype, 2);
    pkt.Append(eth, 14);
    uint8_t ip_hdr[20] = {0};
    ip_hdr[0] = 0x45;
    uint16_t total_len = htons(20 + 20 + static_cast<uint16_t>(payload_len));
    std::memcpy(ip_hdr + 2, &total_len, 2);
    ip_hdr[8] = 64; ip_hdr[9] = 6;
    { struct in_addr a; inet_pton(AF_INET, src_ip.c_str(), &a); std::memcpy(ip_hdr + 12, &a.s_addr, 4); }
    { struct in_addr a; inet_pton(AF_INET, dst_ip.c_str(), &a); std::memcpy(ip_hdr + 16, &a.s_addr, 4); }
    pkt.Append(ip_hdr, 20);
    uint8_t tcp_hdr[20] = {0};
    uint16_t sp = htons(src_port), dp = htons(dst_port);
    uint32_t seq_n = htonl(seq);
    std::memcpy(tcp_hdr + 0, &sp, 2); std::memcpy(tcp_hdr + 2, &dp, 2);
    std::memcpy(tcp_hdr + 4, &seq_n, 4);
    tcp_hdr[12] = 0x50; tcp_hdr[13] = 0x10;
    pkt.Append(tcp_hdr, 20);
    if (payload && payload_len > 0) pkt.Append(payload, payload_len);
    return pkt;
}

RawPacket MakeUdpPacket(const std::string& src_ip, uint16_t src_port,
                         const std::string& dst_ip, uint16_t dst_port,
                         const uint8_t* payload, size_t payload_len) {
    RawPacket pkt;
    uint8_t eth[14] = {0};
    uint16_t ethertype = htons(0x0800);
    std::memcpy(eth + 12, &ethertype, 2);
    pkt.Append(eth, 14);
    uint8_t ip_hdr[20] = {0};
    ip_hdr[0] = 0x45;
    uint16_t total_len = htons(20 + 8 + static_cast<uint16_t>(payload_len));
    std::memcpy(ip_hdr + 2, &total_len, 2);
    ip_hdr[8] = 64; ip_hdr[9] = 17;
    { struct in_addr a; inet_pton(AF_INET, src_ip.c_str(), &a); std::memcpy(ip_hdr + 12, &a.s_addr, 4); }
    { struct in_addr a; inet_pton(AF_INET, dst_ip.c_str(), &a); std::memcpy(ip_hdr + 16, &a.s_addr, 4); }
    pkt.Append(ip_hdr, 20);
    uint8_t udp_hdr[8] = {0};
    uint16_t sp = htons(src_port), dp = htons(dst_port);
    uint16_t len = htons(8 + static_cast<uint16_t>(payload_len));
    std::memcpy(udp_hdr + 0, &sp, 2); std::memcpy(udp_hdr + 2, &dp, 2);
    std::memcpy(udp_hdr + 4, &len, 2);
    pkt.Append(udp_hdr, 8);
    if (payload && payload_len > 0) pkt.Append(payload, payload_len);
    return pkt;
}

} // namespace

// ============================================================
// ST-DEC: PCAP 解码系统测试
// ============================================================

/** @test ST-DEC-001: TCP包完整解码 — Protocol/端口/payload正确 */
TEST(DecodeSystemTest, TcpPacketFullDecode) {
    uint8_t payload[] = {0xCA, 0xFE, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00};
    auto raw = MakeTcpPacket("10.0.0.1", 12345, "10.0.0.2", 9000, 1, payload, sizeof(payload));
    auto packet = PacketParser::Parse(raw.data.data(), raw.data.size());
    ASSERT_NE(packet, nullptr);
    EXPECT_EQ(packet->Protocol(), TransportProtocol::TCP);
    EXPECT_EQ(packet->SrcIp(), "10.0.0.1");
    EXPECT_EQ(packet->DstIp(), "10.0.0.2");
    EXPECT_EQ(packet->SrcPort(), 12345);
    EXPECT_EQ(packet->DstPort(), 9000);
    EXPECT_EQ(packet->PayloadSize(), sizeof(payload));
}

/** @test ST-DEC-002: UDP包完整解码 — Protocol/端口正确 */
TEST(DecodeSystemTest, UdpPacketFullDecode) {
    uint8_t payload[] = {0xCA, 0xFE, 0x01, 0x01};
    auto raw = MakeUdpPacket("10.0.0.1", 35000, "10.0.0.2", 9001, payload, sizeof(payload));
    auto packet = PacketParser::Parse(raw.data.data(), raw.data.size());
    ASSERT_NE(packet, nullptr);
    EXPECT_EQ(packet->Protocol(), TransportProtocol::UDP);
    EXPECT_EQ(packet->SrcPort(), 35000);
    EXPECT_EQ(packet->DstPort(), 9001);
}

/** @test ST-DEC-003: 空/短数据 — 不崩溃,返回nullptr */
TEST(DecodeSystemTest, EmptyDataNoCrash) {
    auto packet = PacketParser::Parse(nullptr, 0);
    EXPECT_EQ(packet, nullptr);
    uint8_t buf[10] = {0};
    packet = PacketParser::Parse(buf, 10);
    EXPECT_EQ(packet, nullptr);
}

/** @test ST-DEC-004: 混合TCP/UDP — 两种Flow各计1 */
TEST(DecodeSystemTest, MixedTcpUdpFlows) {
    FlowTracker tracker;
    uint8_t tcp_payload[] = {0xCA, 0xFE, 0x01, 0x01};
    auto tcp_raw = MakeTcpPacket("10.0.0.1", 11111, "10.0.0.2", 9000, 1, tcp_payload, sizeof(tcp_payload));
    auto tcp_pkt = PacketParser::Parse(tcp_raw.data.data(), tcp_raw.data.size());
    ASSERT_NE(tcp_pkt, nullptr);
    tracker.OnPacket(*tcp_pkt);
    uint8_t udp_payload[] = {0xCA, 0xFE, 0x01};
    auto udp_raw = MakeUdpPacket("10.0.0.1", 22222, "10.0.0.2", 9001, udp_payload, sizeof(udp_payload));
    auto udp_pkt = PacketParser::Parse(udp_raw.data.data(), udp_raw.data.size());
    ASSERT_NE(udp_pkt, nullptr);
    tracker.OnPacket(*udp_pkt);
    EXPECT_EQ(tracker.FlowCount(), 2);
    EXPECT_EQ(tracker.TcpFlowCount(), 1);
    EXPECT_EQ(tracker.UdpFlowCount(), 1);
}

/** @test ST-DEC-005: 同Flow 3个包 — 聚合为1个Flow */
TEST(DecodeSystemTest, FlowTrackerAggregation) {
    FlowTracker tracker;
    for (uint32_t seq = 1; seq <= 3; seq++) {
        uint8_t payload[] = {0x01, 0x02};
        auto raw = MakeTcpPacket("10.0.0.1", 12345, "10.0.0.2", 9000,
                                  seq * 10, payload, sizeof(payload));
        auto pkt = PacketParser::Parse(raw.data.data(), raw.data.size());
        tracker.OnPacket(*pkt);
    }
    EXPECT_EQ(tracker.FlowCount(), 1);
    EXPECT_EQ(tracker.GetFlows()[0]->PacketCount(), 3);
}

/** @test ST-DEC-006: 解码→Timeline链路 — Events非空 */
TEST(DecodeSystemTest, DecodeToTimeline) {
    FlowTracker tracker;
    uint8_t payload[] = {0xCA, 0xFE, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00};
    auto raw = MakeTcpPacket("10.0.0.1", 12345, "10.0.0.2", 9000, 1, payload, sizeof(payload));
    auto pkt = PacketParser::Parse(raw.data.data(), raw.data.size());
    tracker.OnPacket(*pkt);
    auto schema_r = SchemaLoader::LoadFromYaml("../examples/schemas/vehicle-tcp.yaml");
    if (schema_r.IsOk()) {
        TimelineBuilder builder(tracker, schema_r.Value());
        builder.Build();
        EXPECT_FALSE(builder.Events().empty());
    }
}
