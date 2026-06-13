#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

namespace gatewayforge {

struct CommandLine {
    std::string command;
    std::unordered_map<std::string, std::string> options;

    static std::optional<CommandLine> Parse(int argc, char** argv);

    std::optional<std::string> Get(const std::string& key) const;
    std::string GetOr(const std::string& key, const std::string& default_value) const;
};

} // namespace gatewayforge
