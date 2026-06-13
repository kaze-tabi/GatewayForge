#include <gtest/gtest.h>
#include "gatewayforge/protocol/BinaryDecoder.h"

using namespace gatewayforge;

TEST(BinaryDecoderTest, EmptyData) {
    ProtocolSchema schema;
    schema.transport = "udp";
    schema.framing.type = FramingType::DATAGRAM;

    BinaryDecoder decoder(schema);
    auto result = decoder.Decode(nullptr, 0);
    EXPECT_TRUE(result.IsOk());
    EXPECT_EQ(result.Value().size(), 0);
}

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
