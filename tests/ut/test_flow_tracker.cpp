/**
 * @file    tests/ut/test_flow_tracker.cpp
 * @brief   FlowTracker 单元测试 (2 用例)
 * @module  flow
 * @level   Unit Test
 *
 * 对应测试计划: UT-FT-001 ~ UT-FT-002
 */

#include <gtest/gtest.h>
#include "gatewayforge/flow/FlowTracker.h"

using namespace gatewayforge;

/** @test UT-FT-001: 空tracker — FlowCount==0 */
TEST(FlowTrackerTest, EmptyTracker) {
    FlowTracker tracker;
    EXPECT_EQ(tracker.FlowCount(), 0);
    EXPECT_EQ(tracker.TcpFlowCount(), 0);
    EXPECT_EQ(tracker.UdpFlowCount(), 0);
}

/** @test UT-FT-002: TCP包跟踪 — TcpFlowCount==1 */
TEST(FlowTrackerTest, TrackTcpPacket) {
    FlowTracker tracker;
    Packet pkt;
    pkt.SetSrcIp("10.0.0.1");
    pkt.SetSrcPort(12345);
    pkt.SetDstIp("10.0.0.2");
    pkt.SetDstPort(9000);
    pkt.SetProtocol(TransportProtocol::TCP);
    pkt.SetTimestamp(1000);
    pkt.SetPayload(nullptr, 0);
    tracker.OnPacket(pkt);
    EXPECT_EQ(tracker.FlowCount(), 1);
    EXPECT_EQ(tracker.TcpFlowCount(), 1);
}
