#pragma once

#include <cstddef>

// A BufferView is an immutable buffer of bytes.
class BufferView {
 public:
  BufferView(const char* buffer, size_t size);

  // Get raw pointers to the start of the buffer.
  const char* get() const;

  // Access a particular char in the buffer, or throw an exception if the index
  // is out of bounds.
  char at(size_t index) const;

  size_t size() const;

 private:
  const char* buffer_;
  size_t size_;
};

// A Buffer is a mutable buffer of bytes.
class Buffer : public BufferView {
 public:
  Buffer(char* buffer, size_t size);

  using BufferView::get;
  char* get();
  using BufferView::at;
  char& at(size_t index);

  using BufferView::size;

 private:
  char* buffer_;
};

// A FixedBuffer is a mutable buffer of bytes which additionally owns its
// buffer.
template <size_t SIZE>
class FixedBuffer : public Buffer {
 public:
  FixedBuffer() : Buffer(buffer, SIZE) {}

 private:
  char buffer[SIZE];
};
