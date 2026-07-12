/**
 * @file    tests/ut/test_flow_key.cpp
 * @brief   FlowKey 单元测试 (4 用例)
 * @module  flow
 * @level   Unit Test
 *
 * 覆盖场景:
 *   - 相等 / 不等(协议不同)
 *   - 归一化(双向流合并)
 *   - Hash 一致性
 *
 * 对应测试计划: UT-FK-001 ~ UT-FK-004
 */

#include <gtest/gtest.h>
#include "gatewayforge/flow/FlowKey.h"

using namespace gatewayforge;

/** @test UT-FK-001: 相同FlowKey — ==为true */
TEST(FlowKeyTest, Equality) {
    FlowKey a{"10.0.0.1", 1234, "10.0.0.2", 5678, TransportProtocol::TCP};
    FlowKey b{"10.0.0.1", 1234, "10.0.0.2", 5678, TransportProtocol::TCP};
    EXPECT_EQ(a, b);
}

/** @test UT-FK-002: 协议不同 — !=为true */
TEST(FlowKeyTest, Inequality) {
    FlowKey a{"10.0.0.1", 1234, "10.0.0.2", 5678, TransportProtocol::TCP};
    FlowKey b{"10.0.0.1", 1234, "10.0.0.2", 5678, TransportProtocol::UDP};
    EXPECT_NE(a, b);
}

/** @test UT-FK-003: Normalized — 双向流统一 */
TEST(FlowKeyTest, Normalization) {
    FlowKey key{"10.0.0.2", 5678, "10.0.0.1", 1234, TransportProtocol::UDP};
    auto normalized = key.Normalized();
    EXPECT_EQ(normalized.src_ip, "10.0.0.1");
    EXPECT_EQ(normalized.src_port, 1234);
}

/** @test UT-FK-004: Hash — key和Normalized()同hash */
TEST(FlowKeyTest, HashSameForKeyAndNormalized) {
    FlowKey a{"10.0.0.1", 1234, "10.0.0.2", 5678, TransportProtocol::UDP};
    FlowKey b{"10.0.0.2", 5678, "10.0.0.1", 1234, TransportProtocol::UDP};
    EXPECT_EQ(std::hash<FlowKey>{}(a.Normalized()),
              std::hash<FlowKey>{}(b.Normalized()));
}
