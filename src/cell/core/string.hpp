// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_STRING_HPP
#define CELL_STRING_HPP

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "assert.hpp"
#include "string_slice.hpp"

namespace cell {

class String {
 public:
  friend class Scanner;
  friend class StringSlice;

  explicit String() noexcept;
  explicit String(uint64_t initial_capacity_hint) noexcept;
  explicit String(StringSlice slice) noexcept;
  String(const String &other) noexcept;
  String(String &&other) noexcept;
  String &operator=(const String &other) noexcept;
  String &operator=(String &&other) noexcept;
  ~String();

  [[nodiscard]] constexpr uint8_t *get_buffer_ptr() noexcept { return m_buf; }
  [[nodiscard]] constexpr uint8_t *get_buffer_ptr() const noexcept { return m_buf; }
  [[nodiscard]] constexpr uint64_t get_capacity() const noexcept { return m_cap; }
  [[nodiscard]] constexpr uint64_t get_length() const noexcept { return m_len; }
  [[nodiscard]] constexpr uint8_t byte_at(uint64_t i) const noexcept {
    CELL_ASSERT(i < m_len);
    return m_buf[i];
  }

  [[nodiscard]] constexpr bool is_empty() const noexcept {
    return m_len == 0;
  }

  [[nodiscard]] bool compare(StringSlice str) const noexcept;
  [[nodiscard]] bool compare_ignore_case(StringSlice slice) const noexcept;
  [[nodiscard]] bool contains(uint8_t byte) const noexcept;
  [[nodiscard]] bool contains(StringSlice slice) const noexcept;
  [[nodiscard]] bool contains_ignore_case(StringSlice slice) const noexcept;
  [[nodiscard]] bool contains_any_of(StringSlice slice) const noexcept;
  [[nodiscard]] bool contains_any_of(const std::unordered_set<uint8_t> &charset) const noexcept;
  [[nodiscard]] bool contains_just(StringSlice charset) const noexcept;
  [[nodiscard]] bool contains_just(const std::unordered_set<uint8_t> &charset) const noexcept;

  void replace_byte(uint8_t original, uint8_t replacement) noexcept;
  void replace_any_of_chars(StringSlice charset, uint8_t replacement) noexcept;
  void replace(StringSlice candidate, StringSlice replacement) noexcept;
  void replace_any_of(const std::vector<StringSlice> &candidates, StringSlice replacement) noexcept;

  [[nodiscard]] bool starts_with_byte(uint8_t byte) const noexcept;
  [[nodiscard]] bool starts_with_any_of_byte(StringSlice charset) const noexcept;
  [[nodiscard]] bool starts_with(StringSlice slice) const noexcept;
  [[nodiscard]] bool starts_with_ignore_case(StringSlice slice) const noexcept;
  [[nodiscard]] bool ends_with_byte(uint8_t byte) const noexcept;
  [[nodiscard]] bool ends_with_any_of_bytes(StringSlice charset) const noexcept;

  void to_lower() noexcept;
  void to_upper() noexcept;
  void trim(uint8_t delimiter = ' ') noexcept;
  void trim_left(uint8_t delimiter = ' ') noexcept;
  void trim_right(uint8_t delimiter = ' ') noexcept;
  void clear() noexcept;

  void append_byte(uint8_t c) noexcept;
  void append_c_str(const char *cstr) noexcept;
  void append_slice(StringSlice slice) noexcept;
  void append_string(const String &other) noexcept;
  void append_i32(const int32_t num) noexcept { append_sprintf<32>("%ld", num); }
  void append_i64(const int64_t num) noexcept { append_sprintf<32>("%lld", num); }
  void append_u32(const uint32_t num) noexcept { append_sprintf<32>("%lu", num); }
  void append_u64(const uint64_t num) noexcept { append_sprintf<32>("%llu", num); }

  // The printf buffer you will append to is fixed, and is determined by
  // the SprintfBufferSize template argument. Use with caution.
  template <uint64_t SprintfBufferSize, typename... Args>
  void append_sprintf(const char *fmt, Args &&...args) noexcept {
    char tmp[SprintfBufferSize];
    memset(tmp, 0, SprintfBufferSize);
    std::sprintf(tmp, fmt, std::forward<Args>(args)...);
    append_c_str(tmp);
  }

  [[nodiscard]] bool append_file_contents(const char *path) noexcept;
  [[nodiscard]] bool save_to_file(const char *path) const noexcept;

  void expand(uint64_t new_cap);
  void refresh_length() noexcept;

  // Returns a const pointer to the buffer, which is guaranteed to be a C
  // compatible string (null terminated string)
  [[nodiscard]] const char *get_c_str() const noexcept {
    return reinterpret_cast<const char *>(m_buf);
  }

  // Returns a const ptr to the c string, skipping first N delimiters.
  // Note: this function just returns a POINTER to the buffer, and does NOT
  // actually change it. Use with caution.
  [[nodiscard]] const char *get_c_str_trimmed_left(uint8_t delimiter = ' ') const noexcept;

  [[nodiscard]] StringSlice slice() const noexcept;
  [[nodiscard]] StringSlice slice(uint64_t from, uint64_t n) const noexcept;
  [[nodiscard]] StringSlice slice(uint64_t from) const noexcept;

  // Creates new std::string from buffer
  [[nodiscard]] std::string get_std_string() const noexcept { return get_c_str(); }

 private:
  [[nodiscard]] bool impl_compare(const uint8_t *data, uint64_t length, uint64_t offset) const noexcept;
  [[nodiscard]] bool impl_compare_ignore_case(const uint8_t *data, uint64_t length,
                                       uint64_t offset) const noexcept;
  [[nodiscard]] static bool impl_is_any_of(uint8_t candidate, StringSlice charset) noexcept;

  uint64_t m_cap{};
  uint64_t m_len{};
  uint8_t *m_buf{};
};

}  // namespace cell

#endif  // CELL_STRING_HPP
