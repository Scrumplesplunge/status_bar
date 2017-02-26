#include "buffer.h"

#include <stdexcept>

BufferView::BufferView(const char* buffer, size_t size)
    : buffer_(buffer), size_(size) {}

const char* BufferView::get() const { return buffer_; }

char BufferView::at(size_t index) const {
  if (size_ <= index) throw std::out_of_range("Bad index for BufferView.");
  return buffer_[index];
}

size_t BufferView::size() const { return size_; }

Buffer::Buffer(char* buffer, size_t size)
    : BufferView(buffer, size), buffer_(buffer) {}

char* Buffer::get() { return buffer_; }

char& Buffer::at(size_t index) {
  if (size() <= index) throw std::out_of_range("Bad index for Buffer.");
  return buffer_[index];
}
