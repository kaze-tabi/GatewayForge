/**
 * @file    tests/ut/test_pcap_reader.cpp
 * @brief   PcapReader 单元测试 (2 用例)
 * @module  pcap
 * @level   Unit Test
 *
 * 对应测试计划: UT-PR-001 ~ UT-PR-002
 */

#include <gtest/gtest.h>
#include "gatewayforge/pcap/PcapReader.h"

using namespace gatewayforge;

/** @test UT-PR-001: 打开不存在文件 — Open返回false */
TEST(PcapReaderTest, OpenNonExistentFile) {
    PcapReader reader;
    EXPECT_FALSE(reader.Open("nonexistent.pcap"));
}

/** @test UT-PR-002: 默认构造 — IsOpen()为false */
TEST(PcapReaderTest, NotOpenByDefault) {
    PcapReader reader;
    EXPECT_FALSE(reader.IsOpen());
}
