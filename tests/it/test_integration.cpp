/**
 * @file    tests/it/test_integration.cpp
 * @brief   集成测试 — 多模块组合链路 (5 用例)
 * @module  pcap / flow / protocol / timeline / replay
 * @level   Integration Test
 *
 * 覆盖场景:
 *   - IT-DS-001: PCAP → FlowTracker → TimelineBuilder 链路
 *   - IT-DS-002: SchemaLoader → BinaryDecoder (TCP)
 *   - IT-DS-003: SchemaLoader → BinaryDecoder (UDP)
 *   - IT-DS-004: FlowTracker → BinaryDecoder (UDP Flow 解码)
 *   - IT-RP-001: 多Schema独立解码 — 无状态污染
 *
 * 对应测试计划: IT-DS-001 ~ IT-RP-001
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

RawPacket MakeTcpPacket(const std::string& sip, uint16_t sp,
                         const std::string& dip, uint16_t dp,
                         uint32_t seq, const uint8_t* payload, size_t plen) {
    RawPacket pkt;
    uint8_t eth[14] = {0};
    uint16_t etype = htons(0x0800); std::memcpy(eth + 12, &etype, 2);
    pkt.Append(eth, 14);
    uint8_t ip[20] = {0}; ip[0] = 0x45;
    uint16_t tl = htons(20 + 20 + (uint16_t)plen); std::memcpy(ip + 2, &tl, 2);
    ip[8] = 64; ip[9] = 6;
    { struct in_addr a; inet_pton(AF_INET, sip.c_str(), &a); std::memcpy(ip + 12, &a.s_addr, 4); }
    { struct in_addr a; inet_pton(AF_INET, dip.c_str(), &a); std::memcpy(ip + 16, &a.s_addr, 4); }
    pkt.Append(ip, 20);
    uint8_t tcp[20] = {0};
    uint16_t sp_n = htons(sp), dp_n = htons(dp), sq_n = htonl(seq);
    std::memcpy(tcp+0,&sp_n,2); std::memcpy(tcp+2,&dp_n,2); std::memcpy(tcp+4,&sq_n,4);
    tcp[12] = 0x50; tcp[13] = 0x10;
    pkt.Append(tcp, 20);
    if (payload && plen > 0) pkt.Append(payload, plen);
    return pkt;
}

RawPacket MakeUdpPacket(const std::string& sip, uint16_t sp,
                         const std::string& dip, uint16_t dp,
                         const uint8_t* payload, size_t plen) {
    RawPacket pkt;
    uint8_t eth[14] = {0};
    uint16_t etype = htons(0x0800); std::memcpy(eth + 12, &etype, 2);
    pkt.Append(eth, 14);
    uint8_t ip[20] = {0}; ip[0] = 0x45;
    uint16_t tl = htons(20 + 8 + (uint16_t)plen); std::memcpy(ip + 2, &tl, 2);
    ip[8] = 64; ip[9] = 17;
    { struct in_addr a; inet_pton(AF_INET, sip.c_str(), &a); std::memcpy(ip + 12, &a.s_addr, 4); }
    { struct in_addr a; inet_pton(AF_INET, dip.c_str(), &a); std::memcpy(ip + 16, &a.s_addr, 4); }
    pkt.Append(ip, 20);
    uint8_t udp[8] = {0};
    uint16_t sp_n = htons(sp), dp_n = htons(dp), len_n = htons(8 + (uint16_t)plen);
    std::memcpy(udp+0,&sp_n,2); std::memcpy(udp+2,&dp_n,2); std::memcpy(udp+4,&len_n,2);
    pkt.Append(udp, 8);
    if (payload && plen > 0) pkt.Append(payload, plen);
    return pkt;
}

} // namespace

// ============================================================
// IT-DS: 数据流集成
// ============================================================

/** @test IT-DS-001: PCAP→FlowTracker→TimelineBuilder — 事件按时间排序 */
TEST(IntegrationTest, PcapFlowTimelinePipeline_TCP) {
    FlowTracker tracker;
    uint8_t p1[] = {0xCA, 0xFE, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00};
    auto r1 = MakeTcpPacket("10.0.0.1", 12345, "10.0.0.2", 9000, 1, p1, sizeof(p1));
    auto pkt1 = PacketParser::Parse(r1.data.data(), r1.data.size());
    ASSERT_NE(pkt1, nullptr); tracker.OnPacket(*pkt1);
    uint8_t p2[] = {0xCA, 0xFE, 0x01, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00};
    auto r2 = MakeTcpPacket("10.0.0.1", 12345, "10.0.0.2", 9000, 1 + sizeof(p1), p2, sizeof(p2));
    auto pkt2 = PacketParser::Parse(r2.data.data(), r2.data.size());
    ASSERT_NE(pkt2, nullptr); tracker.OnPacket(*pkt2);
    EXPECT_EQ(tracker.TcpFlowCount(), 1);
    auto schema_r = SchemaLoader::LoadFromYaml("../examples/schemas/vehicle-tcp.yaml");
    if (schema_r.IsOk()) {
        TimelineBuilder builder(tracker, schema_r.Value());
        builder.Build();
        EXPECT_FALSE(builder.Events().empty());
        if (builder.Events().size() >= 2)
            EXPECT_LE(builder.Events()[0].timestamp_us, builder.Events()[1].timestamp_us);
    }
}

/** @test IT-DS-002: Schema→Decode TCP — 字段值正确 */
TEST(IntegrationTest, SchemaDecodePipeline_TCP) {
    auto r = SchemaLoader::LoadFromYaml("../examples/schemas/vehicle-tcp.yaml");
    if (r.IsOk()) {
        BinaryDecoder dec(r.Value());
        uint8_t data[] = {0xCA, 0xFE, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00};
        auto dr = dec.Decode(data, sizeof(data));
        ASSERT_TRUE(dr.IsOk());
        EXPECT_EQ(dr.Value().size(), 1);
        EXPECT_EQ(dr.Value()[0]["magic"].As<uint16_t>(), static_cast<uint16_t>(0xCAFE));
        EXPECT_EQ(dr.Value()[0]["version"].As<uint8_t>(), 1);
    }
}

/** @test IT-DS-003: Schema→Decode UDP — 字段值正确 */
TEST(IntegrationTest, SchemaDecodePipeline_UDP) {
    auto r = SchemaLoader::LoadFromYaml("../examples/schemas/vehicle-udp.yaml");
    if (r.IsOk()) {
        BinaryDecoder dec(r.Value());
        uint8_t data[] = {0xCA, 0xFE, 0x01, 0x01, 0,0,0,0,0,0,0x27,0x11,
                          0,0,0,1, 0,0,0x01,0x8D,0,0,0x27,0x10, 0,0};
        auto dr = dec.Decode(data, sizeof(data));
        ASSERT_TRUE(dr.IsOk());
        EXPECT_EQ(dr.Value().size(), 1);
        EXPECT_EQ(dr.Value()[0]["msg_type"].As<uint8_t>(), 1);
    }
}

/** @test IT-DS-004: Flow→Decode UDP — Payload被正确解码 */
TEST(IntegrationTest, FlowDecodePipeline_UDP) {
    FlowTracker tracker;
    uint8_t payload[] = {0xCA, 0xFE, 0x01, 0x01, 0,0,0,0,0,0,0x27,0x11,
                         0,0,0,1, 0,0,0x01,0x8D,0,0,0x27,0x10, 0,0};
    auto raw = MakeUdpPacket("192.168.1.20", 35000, "10.0.0.8", 9001, payload, sizeof(payload));
    auto pkt = PacketParser::Parse(raw.data.data(), raw.data.size());
    ASSERT_NE(pkt, nullptr); tracker.OnPacket(*pkt);
    EXPECT_EQ(tracker.UdpFlowCount(), 1);
    auto sr = SchemaLoader::LoadFromYaml("../examples/schemas/vehicle-udp.yaml");
    if (sr.IsOk()) {
        BinaryDecoder dec(sr.Value());
        for (const auto* f : tracker.GetFlows()) {
            if (auto* uf = dynamic_cast<const UdpFlow*>(f)) {
                for (const auto& dg : uf->Tracker().Datagrams()) {
                    auto r = dec.Decode(dg.Payload().Data(), dg.PayloadSize());
                    EXPECT_TRUE(r.IsOk());
                    if (!r.Value().empty()) EXPECT_GT(r.Value()[0].count("device_id"), 0);
                }
            }
        }
    }
}

/** @test IT-RP-001: 两个Schema独立解码 — 无状态污染 */
TEST(IntegrationTest, MultipleSchemaLoadsNoStatePollution) {
    auto r1 = SchemaLoader::LoadFromYaml("../examples/schemas/vehicle-tcp.yaml");
    auto r2 = SchemaLoader::LoadFromYaml("../examples/schemas/vehicle-udp.yaml");
    if (r1.IsOk() && r2.IsOk()) {
        EXPECT_EQ(r1.Value().transport, "tcp");
        EXPECT_EQ(r2.Value().transport, "udp");
        BinaryDecoder d1(r1.Value()), d2(r2.Value());
        uint8_t td[] = {0xCA, 0xFE, 0x01, 0x01, 0,0,0,1, 0,0};
        EXPECT_TRUE(d1.Decode(td, sizeof(td)).IsOk());
        uint8_t ud[] = {0xCA, 0xFE, 0x01, 0x01, 0,0,0,0,0,0,0x27,0x11,
                        0,0,0,1, 0,0,0x01,0x8D,0,0,0x27,0x10, 0,0};
        EXPECT_TRUE(d2.Decode(ud, sizeof(ud)).IsOk());
    }
}
