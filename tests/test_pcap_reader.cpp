#include <gtest/gtest.h>
#include "gatewayforge/pcap/PcapReader.h"

TEST(PcapReaderTest, OpenNonExistentFile) {
    gatewayforge::PcapReader reader;
    EXPECT_FALSE(reader.Open("nonexistent.pcap"));
}

TEST(PcapReaderTest, NotOpenByDefault) {
    gatewayforge::PcapReader reader;
    EXPECT_FALSE(reader.IsOpen());
}
