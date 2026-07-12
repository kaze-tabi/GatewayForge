#include "gatewayforge/replay/TcpReplayer.h"
#include "gatewayforge/common/Logger.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

namespace gatewayforge {

TcpReplayer::TcpReplayer(const std::string& host, uint16_t port)
    : sock_(-1), connected_(false) {
    sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ < 0) {
        GF_LOG_ERROR("Failed to create TCP socket: {}", strerror(errno));
        return;
    }

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

    if (connect(sock_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        GF_LOG_ERROR("Failed to connect to {}:{}: {}", host, port, strerror(errno));
        close(sock_);
        sock_ = -1;
        return;
    }

    connected_ = true;
    GF_LOG_INFO("TCP connected to {}:{}", host, port);
}

TcpReplayer::~TcpReplayer() {
    if (sock_ >= 0) close(sock_);
}

bool TcpReplayer::Send(const Packet& packet) {
    if (!connected_) return false;
    ssize_t sent = send(sock_, packet.Payload().Data(), packet.PayloadSize(), 0);
    return sent == static_cast<ssize_t>(packet.PayloadSize());
}

} // namespace gatewayforge
