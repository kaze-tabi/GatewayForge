#include "gatewayforge/timeline/TimelineEvent.h"

namespace gatewayforge {

TimelineEvent TimelineEvent::FromMessage(const Message& msg,
                                          const std::string& transport,
                                          const std::string& src,
                                          const std::string& dst) {
    TimelineEvent event;
    event.timestamp_us = msg.timestamp_us;
    event.transport = transport;
    event.src = src;
    event.dst = dst;
    event.message_type = msg.type;
    event.details = msg.summary;
    return event;
}

} // namespace gatewayforge
