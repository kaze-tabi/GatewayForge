#include "gatewayforge/common/Time.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace gatewayforge {

uint64_t Time::NowNs() {
    auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
}

uint64_t Time::NowUs() {
    return NowNs() / 1000;
}

uint64_t Time::NowMs() {
    return NowNs() / 1'000'000;
}

std::string Time::FormatNs(uint64_t ns) {
    double seconds = ns / 1'000'000'000.0;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << seconds << "s";
    return oss.str();
}

std::string Time::FormatUs(uint64_t us) {
    return FormatNs(us * 1000);
}

std::string Time::FormatMs(uint64_t ms) {
    return FormatNs(ms * 1'000'000);
}

} // namespace gatewayforge
