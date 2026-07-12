#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "gatewayforge/protocol/ProtocolSchema.h"
#include "gatewayforge/protocol/FieldValue.h"
#include "gatewayforge/protocol/DecodedMessage.h"
#include "gatewayforge/common/Result.h"
#include "gatewayforge/common/Buffer.h"

namespace gatewayforge {

using DecodedFields = std::vector<std::unordered_map<std::string, FieldValue>>;

class BinaryDecoder {
public:
    explicit BinaryDecoder(const ProtocolSchema& schema);

    Result<DecodedFields> Decode(
        const uint8_t* data, size_t len) const;

    Result<std::vector<Message>> DecodeToMessages(
        const uint8_t* data, size_t len, uint64_t timestamp_us,
        const std::string& direction) const;

private:
    const ProtocolSchema& schema_;
};

} // namespace gatewayforge
