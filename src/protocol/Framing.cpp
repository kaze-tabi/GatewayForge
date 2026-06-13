#include "gatewayforge/protocol/Framing.h"
#include <stdexcept>
#include <unordered_map>

namespace gatewayforge {

FramingType FramingTypeFromString(const std::string& name) {
    if (name == "length_field") return FramingType::LENGTH_FIELD;
    if (name == "datagram") return FramingType::DATAGRAM;
    throw std::runtime_error("Unknown framing type: " + name);
}

} // namespace gatewayforge
