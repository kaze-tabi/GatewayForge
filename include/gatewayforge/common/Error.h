#pragma once

#include <string>
#include <system_error>

namespace gatewayforge {

class Error {
public:
    static std::string FormatErrno(int err);
    static std::string FormatGaiError(int err);

    template<typename... Args>
    static std::string Format(const std::string& fmt, Args&&... args);
};

} // namespace gatewayforge
