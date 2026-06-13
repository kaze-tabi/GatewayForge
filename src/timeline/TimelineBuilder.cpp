#include "gatewayforge/timeline/TimelineBuilder.h"
#include "gatewayforge/flow/TcpFlow.h"
#include "gatewayforge/flow/UdpFlow.h"
#include <algorithm>

namespace gatewayforge {

TimelineBuilder::TimelineBuilder(const FlowTracker& tracker, const ProtocolSchema& schema)
    : tracker_(tracker), schema_(schema) {
    decoder_ = std::make_unique<BinaryDecoder>(schema_);
}

void TimelineBuilder::Build() {
    events_.clear();

    for (const auto* flow : tracker_.GetFlows()) {
        if (auto* tcp = dynamic_cast<const TcpFlow*>(flow)) {
            // TCP: decode stream messages
            auto stream = tcp->ClientToServer().Stream();
            if (!stream.empty()) {
                auto result = decoder_->DecodeToMessages(
                    stream.data(), stream.size(),
                    tcp->FirstSeenUs(), "C->S");
                if (result.IsOk()) {
                    for (const auto& msg : result.Value()) {
                        events_.push_back(TimelineEvent::FromMessage(
                            msg, "TCP",
                            flow->Key().src_ip + ":" + std::to_string(flow->Key().src_port),
                            flow->Key().dst_ip + ":" + std::to_string(flow->Key().dst_port)));
                    }
                }
            }

            auto s_stream = tcp->ServerToClient().Stream();
            if (!s_stream.empty()) {
                auto result = decoder_->DecodeToMessages(
                    s_stream.data(), s_stream.size(),
                    tcp->FirstSeenUs(), "S->C");
                if (result.IsOk()) {
                    for (const auto& msg : result.Value()) {
                        events_.push_back(TimelineEvent::FromMessage(
                            msg, "TCP",
                            flow->Key().dst_ip + ":" + std::to_string(flow->Key().dst_port),
                            flow->Key().src_ip + ":" + std::to_string(flow->Key().src_port)));
                    }
                }
            }
        } else if (auto* udp = dynamic_cast<const UdpFlow*>(flow)) {
            for (const auto& dgram : udp->Tracker().Datagrams()) {
                auto result = decoder_->DecodeToMessages(
                    dgram.Payload().Data(), dgram.PayloadSize(),
                    dgram.TimestampUs(),
                    dgram.SrcIp() + ":" + std::to_string(dgram.SrcPort()) +
                    " -> " + dgram.DstIp() + ":" + std::to_string(dgram.DstPort()));
                if (result.IsOk()) {
                    for (const auto& msg : result.Value()) {
                        events_.push_back(TimelineEvent::FromMessage(
                            msg, "UDP",
                            dgram.SrcIp() + ":" + std::to_string(dgram.SrcPort()),
                            dgram.DstIp() + ":" + std::to_string(dgram.DstPort())));
                    }
                }
            }
        }
    }

    // Sort by timestamp
    std::sort(events_.begin(), events_.end(),
              [](const TimelineEvent& a, const TimelineEvent& b) {
                  return a.timestamp_us < b.timestamp_us;
              });
}

} // namespace gatewayforge
