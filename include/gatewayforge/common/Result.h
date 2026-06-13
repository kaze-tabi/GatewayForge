#pragma once

#include <string>
#include <variant>
#include <optional>
#include <stdexcept>

namespace gatewayforge {

template<typename T>
class Result {
public:
    static Result<T> Ok(T value) { return Result(std::move(value)); }
    static Result<T> Err(std::string error) { return Result(std::move(error), true); }

    bool IsOk() const { return !is_error_; }
    bool IsErr() const { return is_error_; }

    T& Value() { return std::get<T>(data_); }
    const T& Value() const { return std::get<T>(data_); }
    const std::string& Error() const { return std::get<std::string>(data_); }

    T Unwrap() const {
        if (is_error_) throw std::runtime_error(Error());
        return Value();
    }

    T UnwrapOr(T default_value) const {
        return is_error_ ? std::move(default_value) : Value();
    }

private:
    Result(T value) : data_(std::move(value)), is_error_(false) {}
    Result(std::string error, bool) : data_(std::move(error)), is_error_(true) {}

    std::variant<T, std::string> data_;
    bool is_error_;
};

} // namespace gatewayforge
