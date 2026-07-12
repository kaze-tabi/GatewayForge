/**
 * @file    tests/ut/test_framing_type.cpp
 * @brief   FieldType / FramingType / FieldValue 单元测试 (10 用例)
 * @module  protocol
 * @level   Unit Test
 *
 * 覆盖场景:
 *   - FieldType: 10种类型转换 / 未知异常 / ToString 往返
 *   - FieldTypeSize: 标量 / 变长
 *   - FramingType: 合法 / 非法
 *   - FieldValue: uint / string / bytes
 *
 * 对应测试计划: UT-FTY-001 ~ UT-FTY-010
 */

#include <gtest/gtest.h>
#include "gatewayforge/protocol/FieldType.h"
#include "gatewayforge/protocol/Framing.h"
#include "gatewayforge/protocol/FieldValue.h"

using namespace gatewayforge;

// ============================================================
// FieldType
// ============================================================

/** @test UT-FTY-001: 全部10种类型字符串→枚举 */
TEST(FieldTypeTest, AllKnownTypes) {
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

/** @test UT-FTY-002: 未知类型 — 抛出异常 */
TEST(FieldTypeTest, UnknownType_Throws) {
    EXPECT_THROW(FieldTypeFromString("unknown_type"), std::runtime_error);
    EXPECT_THROW(FieldTypeFromString(""), std::runtime_error);
}

/** @test UT-FTY-003: ToString ↔ FromString 往返一致 */
TEST(FieldTypeTest, ToString_RoundTrip) {
    for (auto type : {FieldType::U8, FieldType::U16, FieldType::U32, FieldType::U64,
                      FieldType::I8, FieldType::I16, FieldType::I32, FieldType::I64,
                      FieldType::BYTES, FieldType::STRING}) {
        std::string name = FieldTypeToString(type);
        EXPECT_EQ(FieldTypeFromString(name), type);
    }
}

// ============================================================
// FieldTypeSize
// ============================================================

/** @test UT-FTY-004: 标量类型字节大小 */
TEST(FieldTypeTest, Size_ScalarTypes) {
    EXPECT_EQ(FieldTypeSize(FieldType::U8),  1);
    EXPECT_EQ(FieldTypeSize(FieldType::U16), 2);
    EXPECT_EQ(FieldTypeSize(FieldType::U32), 4);
    EXPECT_EQ(FieldTypeSize(FieldType::U64), 8);
    EXPECT_EQ(FieldTypeSize(FieldType::I8),  1);
    EXPECT_EQ(FieldTypeSize(FieldType::I16), 2);
    EXPECT_EQ(FieldTypeSize(FieldType::I32), 4);
    EXPECT_EQ(FieldTypeSize(FieldType::I64), 8);
}

/** @test UT-FTY-005: bytes/string 大小为 0 */
TEST(FieldTypeTest, Size_VariableTypes_Zero) {
    EXPECT_EQ(FieldTypeSize(FieldType::BYTES),  0);
    EXPECT_EQ(FieldTypeSize(FieldType::STRING), 0);
}

// ============================================================
// FramingType
// ============================================================

/** @test UT-FTY-006: FramingType 合法字符串 */
TEST(FramingTypeTest, FromString_Valid) {
    EXPECT_EQ(FramingTypeFromString("length_field"), FramingType::LENGTH_FIELD);
    EXPECT_EQ(FramingTypeFromString("datagram"),     FramingType::DATAGRAM);
}

/** @test UT-FTY-007: FramingType 非法 — 抛异常 */
TEST(FramingTypeTest, FromString_Invalid) {
    EXPECT_THROW(FramingTypeFromString("invalid"), std::runtime_error);
}

// ============================================================
// FieldValue
// ============================================================

/** @test UT-FTY-008: uint 值存取与 ToString */
TEST(FieldValueTest, UintValues_ToString) {
    FieldValue v8(static_cast<uint8_t>(42));
    EXPECT_EQ(v8.As<uint8_t>(), 42);
    EXPECT_EQ(v8.ToString(), "42");

    FieldValue v16(static_cast<uint16_t>(1000));
    EXPECT_EQ(v16.As<uint16_t>(), 1000);

    FieldValue v32(static_cast<uint32_t>(100000));
    EXPECT_EQ(v32.As<uint32_t>(), 100000);
}

/** @test UT-FTY-009: string 值存取 */
TEST(FieldValueTest, StringValue) {
    FieldValue v(std::string("hello world"));
    EXPECT_EQ(v.As<std::string>(), "hello world");
    EXPECT_EQ(v.ToString(), "hello world");
}

/** @test UT-FTY-010: bytes 值存取 */
TEST(FieldValueTest, BytesValue) {
    std::vector<uint8_t> bytes = {0xDE, 0xAD, 0xBE, 0xEF};
    FieldValue v(bytes);
    EXPECT_EQ(v.As<std::vector<uint8_t>>(), bytes);
    EXPECT_FALSE(v.ToString().empty());
}
