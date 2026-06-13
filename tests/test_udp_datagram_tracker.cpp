#include <gtest/gtest.h>
#include "gatewayforge/flow/UdpDatagramTracker.h"

using namespace gatewayforge;

TEST(UdpDatagramTrackerTest, EmptyTracker) {
    UdpDatagramTracker tracker;
    EXPECT_EQ(tracker.Count(), 0);
    EXPECT_EQ(tracker.ByteCount(), 0);
}

TEST(UdpDatagramTrackerTest, AddDatagram) {
    UdpDatagramTracker tracker;

    Packet pkt;
    uint8_t data[] = {0x01, 0x02};
    pkt.SetPayload(data, sizeof(data));
    tracker.AddDatagram(pkt);

    EXPECT_EQ(tracker.Count(), 1);
    EXPECT_EQ(tracker.ByteCount(), 2);
}
