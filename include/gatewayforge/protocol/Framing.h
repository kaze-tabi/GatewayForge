#pragma once

#include <string>
#include <cstdint>

namespace gatewayforge {

enum class FramingType {
    LENGTH_FIELD,
    DATAGRAM,
};

struct FramingConfig {
    FramingType type;
    std::string length_field;
    size_t header_len;
    bool length_includes_header;

    FramingConfig()
        : type(FramingType::DATAGRAM), header_len(0), length_includes_header(false) {}
};

FramingType FramingTypeFromString(const std::string& name);

} // namespace gatewayforge
