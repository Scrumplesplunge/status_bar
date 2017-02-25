#pragma once

#include <cstddef>

class Buffer {
 public:
  char* get() { return contents_; }
  const char* get() const { return contents_; }

  char& at(size_t index) { return contents_[index]; }
  char at(size_t index) const { return contents_[index]; }

  size_t size() const { return size_; }

 protected:
  size_t size_;
  char* contents_;
};

template <size_t SIZE>
class FixedBuffer : public Buffer {
 public:
  FixedBuffer() { size_ = SIZE; contents_ = buffer; }

 private:
  char buffer[SIZE];
};
