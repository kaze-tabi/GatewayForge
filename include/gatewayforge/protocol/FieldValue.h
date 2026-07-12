#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>
#include <sstream>
#include <iomanip>

namespace gatewayforge {

class FieldValue {
public:
    using VariantType = std::variant<
        uint8_t, uint16_t, uint32_t, uint64_t,
        int8_t, int16_t, int32_t, int64_t,
        std::vector<uint8_t>, std::string>;

    FieldValue() : value_(uint8_t(0)) {}
    explicit FieldValue(VariantType value) : value_(std::move(value)) {}

    template<typename T> T As() const { return std::get<T>(value_); }

    std::string ToString() const {
        return std::visit([](const auto& v) -> std::string {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
                std::ostringstream oss;
                oss << std::hex;
                for (auto b : v) oss << std::setw(2) << std::setfill('0') << (int)b << " ";
                return oss.str();
            } else if constexpr (std::is_same_v<T, std::string>) {
                return v;
            } else {
                return std::to_string(v);
            }
        }, value_);
    }

private:
    VariantType value_;
};

} // namespace gatewayforge
