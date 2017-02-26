#pragma once

#include <cstddef>

// A non-owning buffer class. The methods in this class are named to be
// consistent with other things in the standard library.
class Buffer {
 public:
  Buffer(char* buffer, size_t size);

  // Get raw pointers to the start of the buffer.
  char* get();
  const char* get() const;

  // Access a particular char in the buffer, or throw an exception if the index
  // is out of bounds.
  char& at(size_t index);
  char at(size_t index) const;

  // Returns the size of the buffer.
  size_t size() const;

 private:
  char* buffer_;
  size_t size_;
};

template <size_t SIZE>
class FixedBuffer : public Buffer {
 public:
  FixedBuffer() : Buffer(buffer, SIZE) {}

 private:
  char buffer[SIZE];
};
