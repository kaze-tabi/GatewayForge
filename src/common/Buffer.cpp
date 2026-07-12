#include "gatewayforge/common/Buffer.h"
#include <cstdlib>
#include <cstring>

namespace gatewayforge {

Buffer::Buffer(const uint8_t* data, size_t size) : size_(size) {
    data_ = static_cast<uint8_t*>(std::malloc(size));
    if (data_ && data) {
        std::memcpy(data_, data, size);
    }
}

Buffer::Buffer(size_t size) : size_(size) {
    data_ = static_cast<uint8_t*>(std::malloc(size));
    if (data_) {
        std::memset(data_, 0, size);
    }
}

Buffer::~Buffer() {
    std::free(data_);
}

Buffer::Buffer(const Buffer& other) : size_(other.size_) {
    data_ = static_cast<uint8_t*>(std::malloc(size_));
    if (data_) {
        std::memcpy(data_, other.data_, size_);
    }
}

Buffer& Buffer::operator=(const Buffer& other) {
    if (this != &other) {
        std::free(data_);
        size_ = other.size_;
        data_ = static_cast<uint8_t*>(std::malloc(size_));
        if (data_) {
            std::memcpy(data_, other.data_, size_);
        }
    }
    return *this;
}

Buffer::Buffer(Buffer&& other) noexcept : data_(other.data_), size_(other.size_) {
    other.data_ = nullptr;
    other.size_ = 0;
}

Buffer& Buffer::operator=(Buffer&& other) noexcept {
    if (this != &other) {
        std::free(data_);
        data_ = other.data_;
        size_ = other.size_;
        other.data_ = nullptr;
        other.size_ = 0;
    }
    return *this;
}

uint8_t Buffer::operator[](size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("Buffer::operator[] out of range");
    }
    return data_[index];
}

Buffer Buffer::Slice(size_t offset, size_t length) const {
    if (offset + length > size_) {
        throw std::out_of_range("Buffer::Slice out of range");
    }
    return Buffer(data_ + offset, length);
}

} // namespace gatewayforge
