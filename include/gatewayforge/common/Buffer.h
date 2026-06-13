#pragma once

#include <cstdint>
#include <cstring>
#include <vector>
#include <stdexcept>

namespace gatewayforge {

class Buffer {
public:
    Buffer() : data_(nullptr), size_(0) {}
    Buffer(const uint8_t* data, size_t size);
    explicit Buffer(size_t size);
    ~Buffer();

    Buffer(const Buffer& other);
    Buffer& operator=(const Buffer& other);
    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(Buffer&& other) noexcept;

    const uint8_t* Data() const { return data_; }
    uint8_t* Data() { return data_; }
    size_t Size() const { return size_; }
    bool Empty() const { return size_ == 0; }

    uint8_t operator[](size_t index) const;
    Buffer Slice(size_t offset, size_t length) const;

    template<typename T>
    T Read(size_t offset) const {
        if (offset + sizeof(T) > size_) {
            throw std::out_of_range("Buffer::Read out of range");
        }
        T value;
        std::memcpy(&value, data_ + offset, sizeof(T));
        return value;
    }

private:
    uint8_t* data_;
    size_t size_;
};

} // namespace gatewayforge
