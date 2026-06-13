/**
 * @file    tests/ut/test_packet_parser.cpp
 * @brief   PacketParser 单元测试 (2 用例)
 * @module  pcap
 * @level   Unit Test
 *
 * 对应测试计划: UT-PP-001 ~ UT-PP-002
 */

#include <gtest/gtest.h>
#include "gatewayforge/pcap/PacketParser.h"

using namespace gatewayforge;

/** @test UT-PP-001: nullptr输入 — 返回nullptr */
TEST(PacketParserTest, ParseEmptyData) {
    auto packet = PacketParser::Parse(nullptr, 0);
    EXPECT_EQ(packet, nullptr);
}

/** @test UT-PP-002: 数据太短 — 返回nullptr */
TEST(PacketParserTest, ParseTooSmallData) {
    uint8_t buf[10] = {0};
    auto packet = PacketParser::Parse(buf, 10);
    EXPECT_EQ(packet, nullptr);
}
