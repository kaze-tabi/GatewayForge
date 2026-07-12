/**
 * @file    tests/ut/test_packet.cpp
 * @brief   Packet 类单元测试 (6 用例)
 * @module  pcap
 * @level   Unit Test
 *
 * 覆盖场景:
 *   - 默认构造 / Setter-Getter
 *   - Payload / TCP Flags
 *   - 拷贝构造深拷贝
 *
 * 对应测试计划: UT-PKT-001 ~ UT-PKT-006
 */

#include <gtest/gtest.h>
#include "gatewayforge/pcap/Packet.h"

using namespace gatewayforge;

/** @test UT-PKT-001: 默认构造 — Protocol=UNKNOWN */
TEST(PacketTest, DefaultConstruct_ProtocolUnknown) {
    Packet pkt;
    EXPECT_EQ(pkt.Protocol(), TransportProtocol::UNKNOWN);
    EXPECT_EQ(pkt.SrcPort(), 0);
    EXPECT_EQ(pkt.DstPort(), 0);
}

/** @test UT-PKT-002: Setter/Getter — IP/端口/协议 */
TEST(PacketTest, SettersGetters_IpAndPort) {
    Packet pkt;
    pkt.SetSrcIp("192.168.1.1");
    pkt.SetSrcPort(12345);
    pkt.SetDstIp("10.0.0.1");
    pkt.SetDstPort(9000);
    pkt.SetProtocol(TransportProtocol::TCP);
    EXPECT_EQ(pkt.SrcIp(), "192.168.1.1");
    EXPECT_EQ(pkt.SrcPort(), 12345);
    EXPECT_EQ(pkt.DstIp(), "10.0.0.1");
    EXPECT_EQ(pkt.DstPort(), 9000);
    EXPECT_EQ(pkt.Protocol(), TransportProtocol::TCP);
}

/** @test UT-PKT-003: Payload 设置与读取 */
TEST(PacketTest, Payload_SetAndGet) {
    Packet pkt;
    uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};
    pkt.SetPayload(data, 4);
    EXPECT_EQ(pkt.PayloadSize(), 4);
    EXPECT_EQ(pkt.Payload()[0], 0xDE);
    EXPECT_EQ(pkt.Payload()[3], 0xEF);
}

/** @test UT-PKT-004: TCP SYN flag */
TEST(PacketTest, TcpFlags_Syn) {
    Packet pkt;
    pkt.SetTcpFlags(0x02);
    EXPECT_TRUE(pkt.IsSyn());
    EXPECT_FALSE(pkt.IsFin());
    EXPECT_FALSE(pkt.IsRst());
    EXPECT_FALSE(pkt.IsAck());
}

/** @test UT-PKT-005: TCP FIN+ACK flags */
TEST(PacketTest, TcpFlags_FinAck) {
    Packet pkt;
    pkt.SetTcpFlags(0x11);
    EXPECT_TRUE(pkt.IsFin());
    EXPECT_TRUE(pkt.IsAck());
    EXPECT_FALSE(pkt.IsSyn());
    EXPECT_FALSE(pkt.IsRst());
}

/** @test UT-PKT-006: 拷贝构造 — Payload 深拷贝 */
TEST(PacketTest, CopyConstructor_DeepCopyPayload) {
    Packet a;
    uint8_t data[] = {0x01, 0x02, 0x03};
    a.SetPayload(data, 3);
    a.SetSrcIp("1.1.1.1");
    a.SetSrcPort(1111);
    a.SetTcpSeq(100);
    Packet b(a);
    EXPECT_EQ(b.SrcIp(), "1.1.1.1");
    EXPECT_EQ(b.SrcPort(), 1111);
    EXPECT_EQ(b.TcpSeq(), 100);
    EXPECT_EQ(b.PayloadSize(), 3);
    EXPECT_EQ(b.Payload()[0], 0x01);
}
