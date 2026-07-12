#pragma once

#include <vector>
#include <memory>
#include "gatewayforge/timeline/TimelineEvent.h"
#include "gatewayforge/flow/FlowTracker.h"
#include "gatewayforge/protocol/ProtocolSchema.h"
#include "gatewayforge/protocol/BinaryDecoder.h"

namespace gatewayforge {

class TimelineBuilder {
public:
    TimelineBuilder(const FlowTracker& tracker, const ProtocolSchema& schema);

    void Build();
    const std::vector<TimelineEvent>& Events() const { return events_; }

private:
    const FlowTracker& tracker_;
    const ProtocolSchema& schema_;
    std::unique_ptr<BinaryDecoder> decoder_;
    std::vector<TimelineEvent> events_;
};

} // namespace gatewayforge
