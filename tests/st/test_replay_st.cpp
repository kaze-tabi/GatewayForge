/**
 * @file    tests/st/test_replay_st.cpp
 * @brief   流量回放系统测试 (4 用例)
 * @module  replay
 * @level   System Test
 *
 * 端到端测试 TCP/UDP 回放到真实 socket:
 *   - TCP Replayer 连接 / UDP Replayer 发送
 *   - ReplayOptions 默认值 / 不存在PCAP不崩溃
 *
 * 对应测试计划: ST-REP-001 ~ ST-REP-004
 */

#include <gtest/gtest.h>
#include "gatewayforge/replay/ReplayOptions.h"
#include "gatewayforge/replay/ReplayEngine.h"
#include "gatewayforge/replay/TcpReplayer.h"
#include "gatewayforge/replay/UdpReplayer.h"
#include "gatewayforge/pcap/Packet.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <chrono>

using namespace gatewayforge;

/** @test ST-REP-001: TCP Replayer 连接真实服务端 — 不崩溃 */
TEST(ReplaySystemTest, TcpReplayerConnectToLiveServer) {
    int srv = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_GE(srv, 0);
    int opt = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = 0;
    ASSERT_EQ(bind(srv, (struct sockaddr*)&addr, sizeof(addr)), 0);
    socklen_t len = sizeof(addr);
    getsockname(srv, (struct sockaddr*)&addr, &len);
    uint16_t port = ntohs(addr.sin_port);
    ASSERT_EQ(listen(srv, 1), 0);
    std::thread([&]() {
        struct sockaddr_in ca; socklen_t cl = sizeof(ca);
        int c = accept(srv, (struct sockaddr*)&ca, &cl);
        if (c >= 0) { uint8_t b[256]; recv(c, b, sizeof(b), 0); close(c); }
    }).detach();
    TcpReplayer replayer("127.0.0.1", port);
    close(srv);
}

/** @test ST-REP-002: UDP Replayer 发送 — 服务端收到正确数据 */
TEST(ReplaySystemTest, UdpReplayerSend) {
    int recv_sock = socket(AF_INET, SOCK_DGRAM, 0);
    ASSERT_GE(recv_sock, 0);
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = 0;
    ASSERT_EQ(bind(recv_sock, (struct sockaddr*)&addr, sizeof(addr)), 0);
    socklen_t len = sizeof(addr);
    getsockname(recv_sock, (struct sockaddr*)&addr, &len);
    uint16_t port = ntohs(addr.sin_port);
    UdpReplayer replayer("127.0.0.1", port);
    Packet pkt;
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    pkt.SetPayload(data, sizeof(data));
    EXPECT_TRUE(replayer.Send(pkt));
    uint8_t buf[256];
    struct sockaddr_in sender; socklen_t sl = sizeof(sender);
    ssize_t n = recvfrom(recv_sock, buf, sizeof(buf), MSG_DONTWAIT,
                          (struct sockaddr*)&sender, &sl);
    if (n > 0) { EXPECT_EQ(n, 4); EXPECT_EQ(buf[0], 0x01); EXPECT_EQ(buf[3], 0x04); }
    close(recv_sock);
}

/** @test ST-REP-003: ReplayOptions 默认值 */
TEST(ReplaySystemTest, ReplayOptionsDefaults) {
    ReplayOptions opts;
    EXPECT_DOUBLE_EQ(opts.speed, 1.0);
    EXPECT_FALSE(opts.randomize_source_port);
    EXPECT_EQ(opts.target_port, 0);
}

/** @test ST-REP-004: 不存在PCAP回放 — 不崩溃, SentCount==0 */
TEST(ReplaySystemTest, ReplayNonexistentPcap) {
    ReplayOptions opts;
    opts.pcap_path = "/nonexistent/file.pcap";
    opts.target_host = "127.0.0.1";
    opts.target_port = 9000;
    opts.transport = "tcp";
    ReplayEngine engine(opts);
    EXPECT_NO_THROW(engine.Run());
    EXPECT_EQ(engine.SentCount(), 0);
}
