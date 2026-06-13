/**
 * @file    tests/ut/test_schema_loader.cpp
 * @brief   SchemaLoader / FieldType 单元测试 (4 用例)
 * @module  protocol
 * @level   Unit Test
 *
 * 对应测试计划: UT-SL-001 ~ UT-SL-004
 */

#include <gtest/gtest.h>
#include "gatewayforge/protocol/SchemaLoader.h"
#include "gatewayforge/protocol/FieldType.h"
#include <fstream>

using namespace gatewayforge;

/** @test UT-SL-001: 文件不存在 — IsErr */
TEST(SchemaLoaderTest, LoadNonExistentFile) {
    auto result = SchemaLoader::LoadFromYaml("nonexistent.yaml");
    EXPECT_TRUE(result.IsErr());
}

/** @test UT-SL-002: FieldType — 所有已知类型转换 */
TEST(SchemaLoaderTest, FieldTypeConversion) {
    EXPECT_EQ(FieldTypeFromString("u8"),     FieldType::U8);
    EXPECT_EQ(FieldTypeFromString("u16"),    FieldType::U16);
    EXPECT_EQ(FieldTypeFromString("u32"),    FieldType::U32);
    EXPECT_EQ(FieldTypeFromString("u64"),    FieldType::U64);
    EXPECT_EQ(FieldTypeFromString("i8"),     FieldType::I8);
    EXPECT_EQ(FieldTypeFromString("i16"),    FieldType::I16);
    EXPECT_EQ(FieldTypeFromString("i32"),    FieldType::I32);
    EXPECT_EQ(FieldTypeFromString("i64"),    FieldType::I64);
    EXPECT_EQ(FieldTypeFromString("bytes"),  FieldType::BYTES);
    EXPECT_EQ(FieldTypeFromString("string"), FieldType::STRING);
}

/** @test UT-SL-003: 未知类型 — 抛异常 */
TEST(SchemaLoaderTest, UnknownFieldType) {
    EXPECT_THROW(FieldTypeFromString("unknown"), std::runtime_error);
}

/** @test UT-SL-004: 加载 vehicle-tcp.yaml — IsOk */
TEST(SchemaLoaderTest, LoadTcpSchema) {
    auto result = SchemaLoader::LoadFromYaml("../examples/schemas/vehicle-tcp.yaml");
    if (result.IsOk()) {
        EXPECT_EQ(result.Value().transport, "tcp");
        EXPECT_EQ(result.Value().name, "vehicle_tcp_protocol");
        EXPECT_GT(result.Value().fields.size(), 0);
    }
}
