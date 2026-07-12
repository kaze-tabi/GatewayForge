/**
 * @file    tests/ut/test_command_line.cpp
 * @brief   CommandLine 命令行解析单元测试 (6 用例)
 * @module  cli
 * @level   Unit Test
 *
 * 覆盖场景:
 *   - 无参数 / 命令解析 / 多选项 / flag选项 / GetOr / help
 *
 * 对应测试计划: UT-CL-001 ~ UT-CL-006
 */

#include <gtest/gtest.h>
#include "gatewayforge/cli/CommandLine.h"

using namespace gatewayforge;

/** @test UT-CL-001: 无参数 — nullopt */
TEST(CommandLineTest, NoArguments_ReturnsNullopt) {
    const char* argv[] = {"gatewayforge"};
    auto result = CommandLine::Parse(1, const_cast<char**>(argv));
    EXPECT_FALSE(result.has_value());
}

/** @test UT-CL-002: decode 命令 */
TEST(CommandLineTest, DecodeCommand_Parsed) {
    const char* argv[] = {"gatewayforge", "decode", "--pcap", "test.pcap"};
    auto result = CommandLine::Parse(4, const_cast<char**>(argv));
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->command, "decode");
}

/** @test UT-CL-003: 多选项解析 */
TEST(CommandLineTest, Options_ParsedCorrectly) {
    const char* argv[] = {"gatewayforge", "replay", "--pcap", "a.pcap",
                           "--target", "127.0.0.1:9000", "--speed", "2.0"};
    auto result = CommandLine::Parse(8, const_cast<char**>(argv));
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->command, "replay");
    EXPECT_EQ(result->GetOr("pcap", ""), "a.pcap");
    EXPECT_EQ(result->GetOr("target", ""), "127.0.0.1:9000");
    EXPECT_EQ(result->GetOr("speed", ""), "2.0");
}

/** @test UT-CL-004: Flag 选项无值 — 空字符串 */
TEST(CommandLineTest, FlagWithoutValue_EmptyValue) {
    const char* argv[] = {"gatewayforge", "decode", "--verbose", "--pcap", "f.pcap"};
    auto result = CommandLine::Parse(5, const_cast<char**>(argv));
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->GetOr("verbose", "no"), "");
    EXPECT_EQ(result->GetOr("pcap", ""), "f.pcap");
}

/** @test UT-CL-005: GetOr — 默认值 */
TEST(CommandLineTest, GetOr_DefaultValue) {
    const char* argv[] = {"gatewayforge", "decode"};
    auto result = CommandLine::Parse(2, const_cast<char**>(argv));
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->GetOr("pcap", "default.pcap"), "default.pcap");
    EXPECT_EQ(result->GetOr("speed", "1.0"), "1.0");
}

/** @test UT-CL-006: --help 命令 */
TEST(CommandLineTest, HelpCommand) {
    const char* argv[] = {"gatewayforge", "--help"};
    auto result = CommandLine::Parse(2, const_cast<char**>(argv));
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->command, "--help");
}
