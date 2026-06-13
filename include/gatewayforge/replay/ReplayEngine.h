#pragma once

#include <vector>
#include <memory>
#include "gatewayforge/replay/ReplayOptions.h"
#include "gatewayforge/pcap/Packet.h"

namespace gatewayforge {

class Replayer {
public:
    virtual ~Replayer() = default;
    virtual bool Send(const Packet& packet) = 0;
};

class ReplayEngine {
public:
    explicit ReplayEngine(const ReplayOptions& options);

    void Run();
    size_t SentCount() const { return sent_count_; }

private:
    ReplayOptions options_;
    std::unique_ptr<Replayer> replayer_;
    size_t sent_count_;
};

} // namespace gatewayforge
