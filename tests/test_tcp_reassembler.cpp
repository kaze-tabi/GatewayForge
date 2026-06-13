#include <gtest/gtest.h>
#include "gatewayforge/flow/TcpReassembler.h"

using namespace gatewayforge;

TEST(TcpReassemblerTest, EmptyStream) {
    TcpReassembler reassembler;
    EXPECT_EQ(reassembler.ByteCount(), 0);
}

TEST(TcpReassemblerTest, SingleSegment) {
    TcpReassembler reassembler;

    Packet pkt;
    pkt.SetTcpSeq(1000);
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    pkt.SetPayload(data, sizeof(data));

    reassembler.AddSegment(pkt, true);

    EXPECT_EQ(reassembler.ByteCount(), 4);
    EXPECT_EQ(reassembler.NextSeq(), 1004);
}
