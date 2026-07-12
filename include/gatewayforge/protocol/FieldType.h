#pragma once

#include <string>
#include <cstdint>
#include <stdexcept>

namespace gatewayforge {

enum class FieldType {
    U8,
    U16,
    U32,
    U64,
    I8,
    I16,
    I32,
    I64,
    BYTES,
    STRING,
};

inline size_t FieldTypeSize(FieldType type) {
    switch (type) {
        case FieldType::U8:  case FieldType::I8:      return 1;
        case FieldType::U16: case FieldType::I16:     return 2;
        case FieldType::U32: case FieldType::I32:     return 4;
        case FieldType::U64: case FieldType::I64:     return 8;
        case FieldType::BYTES:  return 0; // variable
        case FieldType::STRING: return 0; // variable
    }
    throw std::runtime_error("Unknown FieldType");
}

FieldType FieldTypeFromString(const std::string& name);
std::string FieldTypeToString(FieldType type);

} // namespace gatewayforge
