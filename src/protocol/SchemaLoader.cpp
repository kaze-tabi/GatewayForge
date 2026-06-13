#include "gatewayforge/protocol/SchemaLoader.h"
#include "gatewayforge/common/Logger.h"
#include <yaml-cpp/yaml.h>
#include <fstream>

namespace gatewayforge {

Result<ProtocolSchema> SchemaLoader::LoadFromYaml(const std::string& filepath) {
    try {
        YAML::Node root = YAML::LoadFile(filepath);

        ProtocolSchema schema;
        schema.name = root["name"].as<std::string>("");
        schema.transport = root["transport"].as<std::string>("tcp");
        schema.endian = root["endian"].as<std::string>("big");

        // Framing
        if (root["framing"]) {
            auto f = root["framing"];
            schema.framing.type = FramingTypeFromString(f["type"].as<std::string>("datagram"));
            if (f["length_field"]) {
                schema.framing.length_field = f["length_field"].as<std::string>();
            }
            if (f["header_len"]) {
                schema.framing.header_len = f["header_len"].as<size_t>();
            }
            if (f["length_includes_header"]) {
                schema.framing.length_includes_header = f["length_includes_header"].as<bool>();
            }
        }

        // Fields
        if (root["message"]) {
            for (const auto& field : root["message"]) {
                FieldDef def;
                def.name = field["name"].as<std::string>();
                def.type = FieldTypeFromString(field["type"].as<std::string>());

                if (field["const"]) {
                    def.has_const = true;
                    def.const_value = field["const"].as<uint64_t>();
                }

                schema.fields.push_back(def);
            }
        }

        // Payload
        if (root["payload"] && root["payload"]["length_from"]) {
            schema.payload.length_from = root["payload"]["length_from"].as<std::string>();
        }

        // Analysis
        if (root["analysis"]) {
            auto a = root["analysis"];
            if (a["sequence_field"]) schema.analysis_sequence_field = a["sequence_field"].as<std::string>();
            if (a["timestamp_field"]) schema.analysis_timestamp_field = a["timestamp_field"].as<std::string>();
            if (a["device_id_field"]) schema.analysis_device_id_field = a["device_id_field"].as<std::string>();
        }

        GF_LOG_INFO("Loaded schema: {} ({} fields, transport={})",
                     schema.name, schema.fields.size(), schema.transport);

        return Result<ProtocolSchema>::Ok(std::move(schema));
    }
    catch (const YAML::Exception& e) {
        return Result<ProtocolSchema>::Err(std::string("YAML parse error: ") + e.what());
    }
    catch (const std::exception& e) {
        return Result<ProtocolSchema>::Err(std::string("Schema load error: ") + e.what());
    }
}

} // namespace gatewayforge
