#include "gatewayforge/timeline/ConsoleTimelinePrinter.h"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace gatewayforge {

void ConsoleTimelinePrinter::Print(const std::vector<TimelineEvent>& events) const {
    if (events.empty()) {
        std::cout << "No events to display.\n";
        return;
    }

    uint64_t base_ts = events[0].timestamp_us;

    for (const auto& event : events) {
        // Format timestamp as MM:SS.mmm
        uint64_t delta_us = event.timestamp_us - base_ts;
        uint64_t minutes = delta_us / 60'000'000;
        uint64_t seconds = (delta_us % 60'000'000) / 1'000'000;
        uint64_t millis = (delta_us % 1'000'000) / 1000;

        std::ostringstream ts;
        ts << std::setfill('0') << std::setw(2) << minutes << ":"
           << std::setfill('0') << std::setw(2) << seconds << "."
           << std::setfill('0') << std::setw(3) << millis;

        std::cout << "[" << ts.str() << "] "
                  << std::setw(4) << std::left << event.transport << " "
                  << std::setw(30) << std::left << (event.src + " -> " + event.dst) << " "
                  << std::setw(16) << std::left << event.message_type << " "
                  << event.details << "\n";
    }
}

} // namespace gatewayforge
