#include "gatewayforge/cli/CommandLine.h"
#include <iostream>

namespace gatewayforge {

std::optional<CommandLine> CommandLine::Parse(int argc, char** argv) {
    if (argc < 2) {
        return std::nullopt;
    }

    CommandLine cmd;
    cmd.command = argv[1];

    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        if (arg.starts_with("--")) {
            std::string key = arg.substr(2);
            std::string value;
            if (i + 1 < argc && !std::string(argv[i + 1]).starts_with("--")) {
                value = argv[i + 1];
                i++;
            }
            cmd.options[key] = value;
        }
    }

    return cmd;
}

std::optional<std::string> CommandLine::Get(const std::string& key) const {
    auto it = options.find(key);
    if (it != options.end()) return it->second;
    return std::nullopt;
}

std::string CommandLine::GetOr(const std::string& key, const std::string& default_value) const {
    auto it = options.find(key);
    return it != options.end() ? it->second : default_value;
}

} // namespace gatewayforge
