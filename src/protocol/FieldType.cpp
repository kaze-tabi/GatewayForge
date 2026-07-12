#include "gatewayforge/protocol/FieldType.h"
#include <unordered_map>

namespace gatewayforge {

FieldType FieldTypeFromString(const std::string& name) {
    static const std::unordered_map<std::string, FieldType> map = {
        {"u8", FieldType::U8},     {"u16", FieldType::U16},
        {"u32", FieldType::U32},   {"u64", FieldType::U64},
        {"i8", FieldType::I8},     {"i16", FieldType::I16},
        {"i32", FieldType::I32},   {"i64", FieldType::I64},
        {"bytes", FieldType::BYTES},
        {"string", FieldType::STRING},
    };
    auto it = map.find(name);
    if (it != map.end()) return it->second;
    throw std::runtime_error("Unknown field type: " + name);
}

std::string FieldTypeToString(FieldType type) {
    switch (type) {
        case FieldType::U8:     return "u8";
        case FieldType::U16:    return "u16";
        case FieldType::U32:    return "u32";
        case FieldType::U64:    return "u64";
        case FieldType::I8:     return "i8";
        case FieldType::I16:    return "i16";
        case FieldType::I32:    return "i32";
        case FieldType::I64:    return "i64";
        case FieldType::BYTES:  return "bytes";
        case FieldType::STRING: return "string";
    }
    return "unknown";
}

} // namespace gatewayforge
