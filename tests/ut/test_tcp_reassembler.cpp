/**
 * @file    tests/ut/test_tcp_reassembler.cpp
 * @brief   TcpReassembler 单元测试 (2 用例)
 * @module  flow
 * @level   Unit Test
 *
 * 对应测试计划: UT-TR-001 ~ UT-TR-002
 */

#include <gtest/gtest.h>
#include "gatewayforge/flow/TcpReassembler.h"

using namespace gatewayforge;

/** @test UT-TR-001: 空重组器 — ByteCount==0 */
TEST(TcpReassemblerTest, EmptyStream) {
    TcpReassembler reassembler;
    EXPECT_EQ(reassembler.ByteCount(), 0);
}

/** @test UT-TR-002: 单段追加 — 字节数和NextSeq正确 */
TEST(TcpReassemblerTest, SingleSegment) {
    TcpReassembler reassembler;
    Packet pkt;
    pkt.SetTcpSeq(1000);
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    pkt.SetPayload(data, sizeof(data));
    reassembler.AddSegment(pkt, true);
    EXPECT_EQ(reassembler.ByteCount(), 4);
    EXPECT_EQ(reassembler.NextSeq(), 1004);
}
