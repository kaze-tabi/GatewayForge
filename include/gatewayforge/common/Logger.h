#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>
#include <string>
#include <mutex>

namespace gatewayforge {

class Logger {
public:
    static void Init(const std::string& level = "info");
    static std::shared_ptr<spdlog::logger> Get() {
        if (!logger_) {
            std::call_once(init_flag_, []() {
                Init("off"); // silent in tests by default
            });
        }
        return logger_;
    }

private:
    static std::shared_ptr<spdlog::logger> logger_;
    static std::once_flag init_flag_;
};

#define GF_LOG_TRACE(...)    gatewayforge::Logger::Get()->trace(__VA_ARGS__)
#define GF_LOG_DEBUG(...)    gatewayforge::Logger::Get()->debug(__VA_ARGS__)
#define GF_LOG_INFO(...)     gatewayforge::Logger::Get()->info(__VA_ARGS__)
#define GF_LOG_WARN(...)     gatewayforge::Logger::Get()->warn(__VA_ARGS__)
#define GF_LOG_ERROR(...)    gatewayforge::Logger::Get()->error(__VA_ARGS__)

} // namespace gatewayforge
