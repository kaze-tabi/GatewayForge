#include "gatewayforge/pcap/PcapReader.h"
#include "gatewayforge/pcap/PacketParser.h"
#include "gatewayforge/common/Logger.h"
#include <pcap.h>
#include <stdexcept>

namespace gatewayforge {

PcapReader::PcapReader() : handle_(nullptr), packet_count_(0) {}

PcapReader::~PcapReader() {
    Close();
}

bool PcapReader::Open(const std::string& filepath) {
    char errbuf[PCAP_ERRBUF_SIZE];
    handle_ = pcap_open_offline(filepath.c_str(), errbuf);
    if (!handle_) {
        GF_LOG_ERROR("Failed to open pcap file: {}", errbuf);
        return false;
    }
    GF_LOG_INFO("Opened pcap file: {}", filepath);
    return true;
}

void PcapReader::Close() {
    if (handle_) {
        pcap_close(handle_);
        handle_ = nullptr;
    }
}

bool PcapReader::IsOpen() const {
    return handle_ != nullptr;
}

void PcapReader::ForEachPacket(const PacketCallback& callback) {
    if (!handle_) return;

    packet_count_ = 0;
    pcap_pkthdr* header;
    const u_char* data;

    while (int ret = pcap_next_ex(handle_, &header, &data) >= 0) {
        if (ret == 0) continue; // timeout

        auto packet = PacketParser::Parse(data, header->len);
        if (packet) {
            packet->SetTimestamp(header->ts.tv_sec * 1'000'000 + header->ts.tv_usec);
            callback(std::move(packet));
        }
        packet_count_++;
    }
}

} // namespace gatewayforge
