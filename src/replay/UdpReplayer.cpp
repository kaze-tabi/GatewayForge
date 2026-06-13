#include "gatewayforge/replay/UdpReplayer.h"
#include "gatewayforge/common/Logger.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

namespace gatewayforge {

UdpReplayer::UdpReplayer(const std::string& host, uint16_t port)
    : sock_(-1), host_(host), port_(port) {
    sock_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_ < 0) {
        GF_LOG_ERROR("Failed to create UDP socket: {}", strerror(errno));
    }
}

UdpReplayer::~UdpReplayer() {
    if (sock_ >= 0) close(sock_);
}

bool UdpReplayer::Send(const Packet& packet) {
    if (sock_ < 0) return false;

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    inet_pton(AF_INET, host_.c_str(), &addr.sin_addr);

    ssize_t sent = sendto(sock_, packet.Payload().Data(), packet.PayloadSize(), 0,
                          (struct sockaddr*)&addr, sizeof(addr));
    return sent == static_cast<ssize_t>(packet.PayloadSize());
}

} // namespace gatewayforge
