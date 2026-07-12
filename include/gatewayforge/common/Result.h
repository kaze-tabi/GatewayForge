#pragma once

#include <string>
#include <stdexcept>
#include <utility>

namespace gatewayforge {

template<typename T>
class Result {
public:
    static Result<T> Ok(T value) {
        Result r;
        r.value_ = std::move(value);
        r.has_value_ = true;
        return r;
    }

    static Result<T> Err(std::string error) {
        Result r;
        r.error_ = std::move(error);
        r.has_value_ = false;
        return r;
    }

    bool IsOk() const { return has_value_; }
    bool IsErr() const { return !has_value_; }

    T& Value() { return value_; }
    const T& Value() const { return value_; }
    const std::string& Error() const { return error_; }

    T Unwrap() const {
        if (!has_value_) throw std::runtime_error(error_);
        return value_;
    }

    T UnwrapOr(T default_value) const {
        return has_value_ ? value_ : std::move(default_value);
    }

private:
    Result() = default;
    T value_{};
    std::string error_;
    bool has_value_ = false;
};

} // namespace gatewayforge
