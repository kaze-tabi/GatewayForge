#include "gatewayforge/protocol/BinaryDecoder.h"
#include "gatewayforge/common/Logger.h"
#include <arpa/inet.h>
#include <cstring>
#include <sstream>

namespace gatewayforge {

using DecodedFields = std::vector<std::unordered_map<std::string, FieldValue>>;

BinaryDecoder::BinaryDecoder(const ProtocolSchema& schema) : schema_(schema) {}

Result<DecodedFields>
BinaryDecoder::Decode(const uint8_t* data, size_t len) const {
    DecodedFields messages;

    if (!data || len == 0) {
        return Result<DecodedFields>::Ok(messages);
    }

    size_t offset = 0;

    if (schema_.transport == "tcp" && schema_.framing.type == FramingType::LENGTH_FIELD) {
        // TCP: length-field framing - may contain multiple messages
        while (offset < len) {
            if (offset + schema_.framing.header_len > len) break;

            size_t msg_start = offset;
            uint16_t payload_len = 0;

            // Find the length field
            for (const auto& field : schema_.fields) {
                size_t field_size = FieldTypeSize(field.type);
                if (offset + field_size > len) break;

                if (field.name == schema_.framing.length_field) {
                    // Read length field
                    if (field.type == FieldType::U16) {
                        uint16_t val;
                        std::memcpy(&val, data + offset, 2);
                        payload_len = ntohs(val);
                    }
                }
                offset += field_size;
            }

            // Allow payload_len == 0: header-only messages are valid

            // Calculate message size
            size_t msg_size = schema_.framing.header_len + payload_len;
            if (!schema_.framing.length_includes_header) {
                msg_size = schema_.framing.header_len + payload_len;
            } else {
                msg_size = payload_len;
            }

            if (msg_start + msg_size > len) break;

            // Decode fields
            std::unordered_map<std::string, FieldValue> message;
            offset = msg_start;
            for (const auto& field : schema_.fields) {
                size_t field_size = FieldTypeSize(field.type);
                if (field.type == FieldType::BYTES || field.type == FieldType::STRING) {
                    field_size = payload_len;
                }
                if (offset + field_size > msg_start + msg_size) break;

                // Read value based on type
                switch (field.type) {
                    case FieldType::U8: {
                        uint8_t v; std::memcpy(&v, data + offset, 1);
                        message[field.name] = FieldValue(v); break;
                    }
                    case FieldType::U16: {
                        uint16_t v; std::memcpy(&v, data + offset, 2);
                        message[field.name] = FieldValue(ntohs(v)); break;
                    }
                    case FieldType::U32: {
                        uint32_t v; std::memcpy(&v, data + offset, 4);
                        message[field.name] = FieldValue(ntohl(v)); break;
                    }
                    case FieldType::U64: {
                        uint64_t v; std::memcpy(&v, data + offset, 8);
                        // For simplicity, keep as-is (no htonll)
                        message[field.name] = FieldValue(v); break;
                    }
                    case FieldType::I8: {
                        int8_t v; std::memcpy(&v, data + offset, 1);
                        message[field.name] = FieldValue(v); break;
                    }
                    case FieldType::I16: {
                        int16_t v_raw; std::memcpy(&v_raw, data + offset, 2);
                        int16_t v = ntohs(v_raw);
                        message[field.name] = FieldValue(v); break;
                    }
                    case FieldType::I32: {
                        int32_t v_raw; std::memcpy(&v_raw, data + offset, 4);
                        int32_t v = ntohl(v_raw);
                        message[field.name] = FieldValue(v); break;
                    }
                    case FieldType::I64: {
                        int64_t v; std::memcpy(&v, data + offset, 8);
                        message[field.name] = FieldValue(v); break;
                    }
                    case FieldType::BYTES: {
                        std::vector<uint8_t> v(data + offset, data + offset + payload_len);
                        message[field.name] = FieldValue(std::move(v)); break;
                    }
                    case FieldType::STRING: {
                        std::string str(reinterpret_cast<const char*>(data + offset), payload_len);
                        message[field.name] = FieldValue(std::move(str)); break;
                    }
                }
                offset += field_size;
            }

            messages.push_back(std::move(message));
            offset = msg_start + msg_size;
        }
    } else if (schema_.transport == "udp") {
        // UDP: each datagram is one message
        std::unordered_map<std::string, FieldValue> message;
        for (const auto& field : schema_.fields) {
            size_t field_size = FieldTypeSize(field.type);
            if (field.type == FieldType::BYTES || field.type == FieldType::STRING) {
                field_size = len - offset;
            }
            if (offset + field_size > len) break;

            switch (field.type) {
                case FieldType::U8: {
                    uint8_t v; std::memcpy(&v, data + offset, 1);
                    message[field.name] = FieldValue(v); break;
                }
                case FieldType::U16: {
                    uint16_t v; std::memcpy(&v, data + offset, 2);
                    message[field.name] = FieldValue(ntohs(v)); break;
                }
                case FieldType::U32: {
                    uint32_t v; std::memcpy(&v, data + offset, 4);
                    message[field.name] = FieldValue(ntohl(v)); break;
                }
                case FieldType::U64: {
                    uint64_t v; std::memcpy(&v, data + offset, 8);
                    message[field.name] = FieldValue(v); break;
                }
                case FieldType::I8: {
                    int8_t v; std::memcpy(&v, data + offset, 1);
                    message[field.name] = FieldValue(v); break;
                }
                case FieldType::I16: {
                    int16_t v_raw; std::memcpy(&v_raw, data + offset, 2);
                    message[field.name] = FieldValue(static_cast<int16_t>(ntohs(v_raw))); break;
                }
                case FieldType::I32: {
                    int32_t v_raw; std::memcpy(&v_raw, data + offset, 4);
                    message[field.name] = FieldValue(static_cast<int32_t>(ntohl(v_raw))); break;
                }
                case FieldType::I64: {
                    int64_t v; std::memcpy(&v, data + offset, 8);
                    message[field.name] = FieldValue(v); break;
                }
                case FieldType::BYTES: {
                    std::vector<uint8_t> v(data + offset, data + offset + (len - offset));
                    message[field.name] = FieldValue(std::move(v)); break;
                }
                case FieldType::STRING: {
                    std::string str(reinterpret_cast<const char*>(data + offset), len - offset);
                    message[field.name] = FieldValue(std::move(str)); break;
                }
            }
            offset += field_size;
        }
        messages.push_back(std::move(message));
    }

    return Result<DecodedFields>::Ok(std::move(messages));
}

Result<std::vector<Message>> BinaryDecoder::DecodeToMessages(
    const uint8_t* data, size_t len,
    uint64_t timestamp_us, const std::string& direction) const {

    auto result = Decode(data, len);
    if (result.IsErr()) {
        return Result<std::vector<Message>>::Err(result.Error());
    }

    std::vector<Message> messages;
    for (const auto& decoded : result.Value()) {
        Message msg;
        msg.timestamp_us = timestamp_us;
        msg.direction = direction;

        // Determine message type
        auto type_it = decoded.find("msg_type");
        if (type_it != decoded.end()) {
            msg.type = type_it->second.ToString();
        } else {
            msg.type = "DATA";
        }

        // Build summary
        std::ostringstream oss;
        for (const auto& pair : decoded) {
            if (pair.first != "magic") {
                oss << pair.first << "=" << pair.second.ToString() << " ";
            }
        }
        msg.summary = oss.str();
        messages.push_back(std::move(msg));
    }

    return Result<std::vector<Message>>::Ok(std::move(messages));
}

} // namespace gatewayforge
