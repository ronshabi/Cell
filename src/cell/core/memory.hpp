// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_MEMORY_HPP
#define CELL_MEMORY_HPP

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "assert.hpp"

namespace cell {

template <typename T>
[[nodiscard]] inline T* mem_alloc(const uint64_t bytes) noexcept {
  CELL_ASSERT(bytes != 0);

  T* ptr = reinterpret_cast<T*>(::malloc(bytes));
  if (ptr == nullptr) [[unlikely]] {
    CELL_PANIC("cell::mem_alloc failed");
  }

  return ptr;
}

template <typename T>
[[nodiscard]] inline T* mem_realloc(T* ptr, uint64_t new_size) noexcept {
  if (ptr == nullptr) [[unlikely]] {
    CELL_PANIC("cell::mem_realloc was called with a null pointer")
  }

  CELL_ASSERT(new_size != 0);

  T* new_ptr = reinterpret_cast<T*>(::realloc(ptr, new_size));
  if (new_ptr == nullptr) [[unlikely]] {
    CELL_PANIC("cell::mem_realloc failed")
  }

  return new_ptr;
}

inline void mem_free(void* ptr) noexcept {
  if (ptr) [[likely]] {
    ::free(ptr);
  }
}

inline void mem_zero(void* ptr, const uint64_t length) noexcept {
  if (!ptr) [[unlikely]] {
    CELL_PANIC("cell::mem_zero was called with nullptr")
  }

  CELL_ASSERT(length != 0);
  ::memset(ptr, 0, length);
}

// Zero all memory in range [ptr0, ptr1)
// Like STL Iterators, ptr1 should be 1 byte after the last byte to be zeroed
inline void mem_zero_ptr_range(void* ptr0, void* ptr1) noexcept {
  if (ptr0 == nullptr || ptr1 == nullptr) [[unlikely]] {
    CELL_PANIC("cell::mem_zero_ptr_range was called with a nullptr")
  }

  CELL_ASSERT(ptr0 != ptr1);
  ::memset(ptr0, 0, static_cast<size_t>(static_cast<char*>(ptr1) - static_cast<char*>(ptr0)));
}

inline bool mem_compare(const void* ptr0, const void* ptr1, const uint64_t length) noexcept {
  if (ptr0 == nullptr || ptr1 == nullptr) [[unlikely]] {
    CELL_PANIC("cell::MemCpy was called with a nullptr")
  }

  CELL_ASSERT(length != 0);
  return ::memcmp(ptr0, ptr1, length) == 0;
}

inline void mem_copy(void* dest, const void* src, const uint64_t length) noexcept {
  if (dest == nullptr || src == nullptr) [[unlikely]] {
    CELL_PANIC("cell::MemCpy was called with a nullptr")
  }

  CELL_ASSERT(length != 0);
  ::memcpy(dest, src, length);
}

[[nodiscard]] inline uint64_t string_length(const char* str) noexcept { return strlen(str); }

}  // namespace cell

#endif  // CELL_MEMORY_HPP
