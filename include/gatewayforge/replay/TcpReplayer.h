#pragma once

#include <string>
#include <cstdint>
#include "gatewayforge/replay/ReplayEngine.h"

namespace gatewayforge {

class TcpReplayer : public Replayer {
public:
    TcpReplayer(const std::string& host, uint16_t port);
    ~TcpReplayer() override;

    bool Send(const Packet& packet) override;

private:
    int sock_;
    bool connected_;
};

} // namespace gatewayforge
