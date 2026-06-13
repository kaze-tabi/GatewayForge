#pragma once

#include <vector>
#include "gatewayforge/timeline/TimelineEvent.h"

namespace gatewayforge {

class ConsoleTimelinePrinter {
public:
    void Print(const std::vector<TimelineEvent>& events) const;
};

} // namespace gatewayforge
