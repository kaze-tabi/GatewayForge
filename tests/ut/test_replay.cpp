/**
 * @file    tests/ut/test_replay.cpp
 * @brief   ReplayOptions 单元测试 (1 用例)
 * @module  replay
 * @level   Unit Test
 *
 * 对应测试计划: UT-RP-001
 */

#include <gtest/gtest.h>
#include "gatewayforge/replay/ReplayOptions.h"

using namespace gatewayforge;

/** @test UT-RP-001: ReplayOptions 默认值 */
TEST(ReplayOptionsTest, DefaultValues) {
    ReplayOptions options;
    EXPECT_DOUBLE_EQ(options.speed, 1.0);
    EXPECT_FALSE(options.randomize_source_port);
}
