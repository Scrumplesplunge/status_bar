#include "buffer.h"

Buffer::Buffer(char* buffer, size_t size)
    : buffer_(buffer), size_(size) {}

char* Buffer::get() { return buffer_; }
const char* Buffer::get() const { return buffer_; }

char& Buffer::at(size_t index) { return buffer_[index]; }
char Buffer::at(size_t index) const { return buffer_[index]; }

size_t Buffer::size() const { return size_; }
