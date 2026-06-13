#include "gatewayforge/common/Logger.h"
#include <mutex>

namespace gatewayforge {

std::shared_ptr<spdlog::logger> Logger::logger_;
std::once_flag Logger::init_flag_;

void Logger::Init(const std::string& level) {
    logger_ = spdlog::stdout_color_mt("gatewayforge");
    if (level == "trace") logger_->set_level(spdlog::level::trace);
    else if (level == "debug") logger_->set_level(spdlog::level::debug);
    else if (level == "warn") logger_->set_level(spdlog::level::warn);
    else if (level == "error") logger_->set_level(spdlog::level::err);
    else logger_->set_level(spdlog::level::info);
}

} // namespace gatewayforge
