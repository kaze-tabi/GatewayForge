#pragma once

#include <string>
#include <functional>
#include <memory>
#include "gatewayforge/pcap/Packet.h"
#include <pcap.h>

namespace gatewayforge {

class PcapReader {
public:
    using PacketCallback = std::function<void(std::unique_ptr<Packet>)>;

    PcapReader();
    ~PcapReader();

    bool Open(const std::string& filepath);
    void Close();
    bool IsOpen() const;

    void ForEachPacket(const PacketCallback& callback);
    size_t PacketCount() const { return packet_count_; }

private:
    pcap_t* handle_;
    size_t packet_count_;
};

} // namespace gatewayforge
