#include "gatewayforge/flow/UdpDatagramTracker.h"

namespace gatewayforge {

void UdpDatagramTracker::AddDatagram(const Packet& packet) {
    datagrams_.push_back(packet);
    // MVP: simple datagram collection, advanced analysis deferred
}

size_t UdpDatagramTracker::ByteCount() const {
    size_t total = 0;
    for (const auto& d : datagrams_) {
        total += d.PayloadSize();
    }
    return total;
}

double UdpDatagramTracker::LossRate() const {
    size_t total = Count();
    if (total == 0) return 0.0;
    return static_cast<double>(missing_seq_count_) / total;
}

double UdpDatagramTracker::ReorderRate() const {
    size_t total = Count();
    if (total == 0) return 0.0;
    return static_cast<double>(out_of_order_count_) / total;
}

} // namespace gatewayforge
