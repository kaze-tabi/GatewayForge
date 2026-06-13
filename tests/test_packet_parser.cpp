#include <gtest/gtest.h>
#include "gatewayforge/pcap/PacketParser.h"

TEST(PacketParserTest, ParseEmptyData) {
    auto packet = gatewayforge::PacketParser::Parse(nullptr, 0);
    EXPECT_EQ(packet, nullptr);
}

TEST(PacketParserTest, ParseTooSmallData) {
    uint8_t buf[10] = {0};
    auto packet = gatewayforge::PacketParser::Parse(buf, 10);
    EXPECT_EQ(packet, nullptr);
}
