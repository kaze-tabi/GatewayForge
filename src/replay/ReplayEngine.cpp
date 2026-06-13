#include "gatewayforge/replay/ReplayEngine.h"
#include "gatewayforge/replay/TcpReplayer.h"
#include "gatewayforge/replay/UdpReplayer.h"
#include "gatewayforge/pcap/PcapReader.h"
#include "gatewayforge/common/Logger.h"
#include "gatewayforge/common/Time.h"
#include <thread>
#include <chrono>

namespace gatewayforge {

ReplayEngine::ReplayEngine(const ReplayOptions& options)
    : options_(options), sent_count_(0) {
    if (options.transport == "tcp") {
        replayer_ = std::make_unique<TcpReplayer>(options.target_host, options.target_port);
    } else {
        replayer_ = std::make_unique<UdpReplayer>(options.target_host, options.target_port);
    }
}

void ReplayEngine::Run() {
    PcapReader reader;
    if (!reader.Open(options_.pcap_path)) {
        GF_LOG_ERROR("Failed to open pcap: {}", options_.pcap_path);
        return;
    }

    GF_LOG_INFO("Starting replay: {} -> {}:{} ({}x speed)",
                options_.pcap_path, options_.target_host,
                options_.target_port, options_.speed);

    uint64_t base_ts = 0;
    reader.ForEachPacket([&](std::unique_ptr<Packet> packet) {
        if (!packet || packet->PayloadSize() == 0) return;
        if (packet->Protocol() != TransportProtocol::TCP &&
            packet->Protocol() != TransportProtocol::UDP) return;

        if (base_ts == 0) {
            base_ts = packet->TimestampUs();
        }

        // Timing control
        uint64_t elapsed_us = packet->TimestampUs() - base_ts;
        uint64_t wait_us = static_cast<uint64_t>(elapsed_us / options_.speed);
        auto now = Time::NowUs();
        static uint64_t start_us = now;
        uint64_t target_us = start_us + wait_us;
        if (now < target_us) {
            std::this_thread::sleep_for(std::chrono::microseconds(target_us - now));
        }

        if (replayer_->Send(*packet)) {
            sent_count_++;
        }
    });

    GF_LOG_INFO("Replay complete. Sent {} packets.", sent_count_);
}

} // namespace gatewayforge
