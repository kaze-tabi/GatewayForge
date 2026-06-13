#pragma once

#include <string>
#include "gatewayforge/protocol/ProtocolSchema.h"
#include "gatewayforge/common/Result.h"

namespace gatewayforge {

class SchemaLoader {
public:
    static Result<ProtocolSchema> LoadFromYaml(const std::string& filepath);
};

} // namespace gatewayforge
