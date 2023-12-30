// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_STRING_SLICE_HPP
#define CELL_STRING_SLICE_HPP

#include <cstdint>

#include "assert.hpp"

namespace cell {

class StringSlice {
 public:
  explicit StringSlice(const uint8_t* data);
  StringSlice(const uint8_t* data, uint64_t len);

  StringSlice(const StringSlice& other) = default;
  StringSlice(StringSlice&& other) = default;
  StringSlice& operator=(const StringSlice& other) = default;
  StringSlice& operator=(StringSlice&& other) = default;

  [[nodiscard]] static StringSlice from_cstr(const char* data) noexcept;
  [[nodiscard]] static StringSlice from_cstr(const char* data, uint64_t len) noexcept;

  [[nodiscard]] const char* get_const_char_ptr() const { return reinterpret_cast<const char*>(m_data); }
  [[nodiscard]] const uint8_t* get_u8_ptr() const { return m_data; }
  [[nodiscard]] uint64_t get_length() const { return m_len; }
  [[nodiscard]] uint8_t byte_at(uint64_t i) const {
    CELL_ASSERT(i < m_len);

    return m_data[i];
  }

  [[nodiscard]] bool compare(StringSlice to) const noexcept;
  [[nodiscard]] bool compare_ignore_case(StringSlice to) const noexcept;
  [[nodiscard]] bool contains(uint8_t byte) const noexcept;

 private:
  const uint8_t* m_data{nullptr};
  uint64_t m_len{0};
};

}  // namespace cell

#endif  // CELL_STRING_SLICE_HPP
