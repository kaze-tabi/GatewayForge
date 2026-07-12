/**
 * @file    tests/ut/test_udp_datagram_tracker.cpp
 * @brief   UdpDatagramTracker 单元测试 (2 用例)
 * @module  flow
 * @level   Unit Test
 *
 * 对应测试计划: UT-UDT-001 ~ UT-UDT-002
 */

#include <gtest/gtest.h>
#include "gatewayforge/flow/UdpDatagramTracker.h"

using namespace gatewayforge;

/** @test UT-UDT-001: 空tracker — Count==0, ByteCount==0 */
TEST(UdpDatagramTrackerTest, EmptyTracker) {
    UdpDatagramTracker tracker;
    EXPECT_EQ(tracker.Count(), 0);
    EXPECT_EQ(tracker.ByteCount(), 0);
}

/** @test UT-UDT-002: 追加datagram — Count==1, ByteCount==2 */
TEST(UdpDatagramTrackerTest, AddDatagram) {
    UdpDatagramTracker tracker;
    Packet pkt;
    uint8_t data[] = {0x01, 0x02};
    pkt.SetPayload(data, sizeof(data));
    tracker.AddDatagram(pkt);
    EXPECT_EQ(tracker.Count(), 1);
    EXPECT_EQ(tracker.ByteCount(), 2);
}
