#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "gatewayforge/protocol/FieldType.h"
#include "gatewayforge/protocol/Framing.h"

namespace gatewayforge {

struct FieldDef {
    std::string name;
    FieldType type;
    uint64_t const_value;
    bool has_const;

    FieldDef() : type(FieldType::U8), const_value(0), has_const(false) {}
};

struct PayloadDef {
    std::string length_from;
};

struct ProtocolSchema {
    std::string name;
    std::string transport;
    std::string endian;
    FramingConfig framing;
    std::vector<FieldDef> fields;
    PayloadDef payload;
    std::string analysis_sequence_field;
    std::string analysis_timestamp_field;
    std::string analysis_device_id_field;
};

} // namespace gatewayforge
