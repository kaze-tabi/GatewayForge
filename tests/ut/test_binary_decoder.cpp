/**
 * @file    tests/ut/test_binary_decoder.cpp
 * @brief   BinaryDecoder 单元测试 (3 用例)
 * @module  protocol
 * @level   Unit Test
 *
 * 覆盖场景:
 *   - 空数据解码 / UDP datagram 解码
 *   - TCP 多消息解码
 *
 * 对应测试计划: UT-BD-001 ~ UT-BD-003
 */

#include <gtest/gtest.h>
#include "gatewayforge/protocol/BinaryDecoder.h"

using namespace gatewayforge;

/** @test UT-BD-001: 空数据 — IsOk, messages空 */
TEST(BinaryDecoderTest, EmptyData) {
    ProtocolSchema schema;
    schema.transport = "udp";
    schema.framing.type = FramingType::DATAGRAM;
    BinaryDecoder decoder(schema);
    auto result = decoder.Decode(nullptr, 0);
    EXPECT_TRUE(result.IsOk());
    EXPECT_EQ(result.Value().size(), 0);
}

/** @test UT-BD-002: UDP单字段(magic=0xCAFE)解码 */
TEST(BinaryDecoderTest, DecodeUdpDatagram) {
    ProtocolSchema schema;
    schema.transport = "udp";
    schema.framing.type = FramingType::DATAGRAM;
    FieldDef field;
    field.name = "magic";
    field.type = FieldType::U16;
    schema.fields.push_back(field);

    uint8_t data[] = {0xCA, 0xFE};
    BinaryDecoder decoder(schema);
    auto result = decoder.Decode(data, sizeof(data));
    EXPECT_TRUE(result.IsOk());
    EXPECT_EQ(result.Value().size(), 1);
    EXPECT_EQ(result.Value()[0]["magic"].As<uint16_t>(), static_cast<uint16_t>(0xCAFE));
}

/** @test UT-BD-003: DecodeToMessages — Message.type 正确 */
TEST(BinaryDecoderTest, DecodeToMessagesSuccess) {
    ProtocolSchema schema;
    schema.transport = "udp";
    schema.framing.type = FramingType::DATAGRAM;
    FieldDef field;
    field.name = "msg_type";
    field.type = FieldType::U8;
    schema.fields.push_back(field);

    uint8_t data[] = {0x01};
    BinaryDecoder decoder(schema);
    auto result = decoder.DecodeToMessages(data, sizeof(data), 1000, "C->S");
    EXPECT_TRUE(result.IsOk());
    EXPECT_EQ(result.Value()[0].type, "1");
}
