#pragma once

#include <string>
#include <cstdint>
#include "gatewayforge/replay/ReplayEngine.h"

namespace gatewayforge {

class UdpReplayer : public Replayer {
public:
    UdpReplayer(const std::string& host, uint16_t port);
    ~UdpReplayer() override;

    bool Send(const Packet& packet) override;

private:
    int sock_;
    std::string host_;
    uint16_t port_;
};

} // namespace gatewayforge
