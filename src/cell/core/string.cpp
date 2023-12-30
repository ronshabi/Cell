// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include "string.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdint>

#include "assert.hpp"
#include "base.hpp"
#include "charset.hpp"
#include "memory.hpp"
#include "string_slice.hpp"
#include "weak_string_cache.hpp"

namespace cell {

// -----------------------------------------------------------------------------
// Constructors/Destructors
// -----------------------------------------------------------------------------

String::String() noexcept : String(8) {}

String::String(uint64_t initial_capacity_hint) noexcept
    : m_cap(round_up_8(initial_capacity_hint)), m_len(0), m_buf(mem_alloc<uint8_t>(m_cap)) {
  mem_zero(m_buf, m_cap);
}

String::String(StringSlice slice) noexcept
    : m_cap(round_up_8(slice.get_length())), m_len(slice.get_length()), m_buf(mem_alloc<uint8_t>(m_cap)) {
  mem_zero(m_buf, m_cap);
  mem_copy(m_buf, slice.GetU8Ptr(), m_len);
}

String::String(const String &other) noexcept
    : m_cap(other.m_cap), m_len(other.m_len), m_buf(mem_alloc<uint8_t>(other.m_cap)) {
  mem_copy(m_buf, other.m_buf, other.m_cap);
}

String::String(String &&other) noexcept : m_cap(other.m_cap), m_len(other.m_len), m_buf(other.m_buf) {
  CELL_ASSERT(other.m_buf != nullptr);
  other.m_buf = nullptr;
}

String::~String() { mem_free(m_buf); }

// -----------------------------------------------------------------------------
// Assignment Operators
// -----------------------------------------------------------------------------

String &String::operator=(const String &other) noexcept {
  CELL_ASSERT(other.m_buf != nullptr);
  expand(other.m_cap);
  m_len = other.m_len;
  mem_copy(m_buf, other.m_buf, other.m_len);

  return *this;
}

String &String::operator=(String &&other) noexcept {
  CELL_ASSERT(other.m_buf != nullptr);
  mem_zero(m_buf, m_cap);
  mem_free(m_buf);
  m_cap = other.m_cap;
  m_len = other.m_len;
  m_buf = other.m_buf;

  CELL_LOG_DEBUG("!!!! IS THIS??? -- %s", other.m_buf);
  other.m_buf = nullptr;

  return *this;
}

// -----------------------------------------------------------------------------
// Public Functions
// -----------------------------------------------------------------------------

bool String::compare(StringSlice str) const noexcept {
  return m_len == str.get_length() && impl_compare(str.GetU8Ptr(), m_len, 0);
}

bool String::compare_ignore_case(StringSlice slice) const noexcept {
  return m_len == slice.get_length() && impl_compare_ignore_case(slice.GetU8Ptr(), m_len, 0);
}

bool String::contains(uint8_t byte) const noexcept {
  for (uint64_t i = 0; i < m_len; i++) {
    if (m_buf[i] == byte) {
      return true;
    }
  }

  return false;
}

bool String::contains(StringSlice slice) const noexcept {
  for (uint64_t i = 0; i < m_len; i++) {
    if (impl_compare(slice.GetU8Ptr(), slice.get_length(), i)) {
      return true;
    }
  }

  return false;
}

bool String::contains_ignore_case(StringSlice slice) const noexcept {
  for (uint64_t i = 0; i < m_len; i++) {
    if (impl_compare_ignore_case(slice.GetU8Ptr(), slice.get_length(), i)) {
      return true;
    }
  }

  return false;
}

bool String::contains_any_of(StringSlice slice) const noexcept {
  for (uint64_t i = 0; i < m_len; i++) {
    if (impl_is_any_of(m_buf[i], slice)) {
      return true;
    }
  }

  return false;
}

bool String::contains_any_of(const std::unordered_set<uint8_t> &charset) const noexcept {
  for (uint64_t i = 0; i < m_len; i++) {
    if (charset.contains(m_buf[i])) {
      return true;
    }
  }

  return false;
}

bool String::contains_just(StringSlice charset) const noexcept {
  for (uint64_t i = 0; i < m_len; i++) {
    if (!impl_is_any_of(m_buf[i], charset)) {
      return false;
    }
  }

  return true;
}

bool String::contains_just(const std::unordered_set<uint8_t> &charset) const noexcept {
  for (uint64_t i = 0; i < m_len; i++) {
    if (!charset.contains(m_buf[i])) {
      return false;
    }
  }

  return true;
}

void String::replace_byte(uint8_t original, uint8_t replacement) noexcept {
  for (uint64_t i = 0; i < m_len; i++) {
    if (m_buf[i] == original) {
      m_buf[i] = replacement;
    }
  }
}

void String::replace_any_of_chars(StringSlice charset, uint8_t replacement) noexcept {
  for (uint64_t i = 0; i < m_len; i++) {
    if (impl_is_any_of(m_buf[i], charset)) {
      m_buf[i] = replacement;
    }
  }
}

void String::replace(StringSlice candidate, StringSlice replacement) noexcept {
  String modified(m_cap);

  uint64_t i = 0;
  while (i < m_len) {
    if (impl_compare(candidate.GetU8Ptr(), candidate.get_length(), i)) {
      modified.append_slice(replacement);
      i += candidate.get_length();
    } else {
      modified.append_byte(m_buf[i]);
      ++i;
    }
  }

  *this = std::move(modified);
}

void String::replace_any_of(const std::vector<StringSlice> &candidates,
                          StringSlice replacement) noexcept {
  if (candidates.empty()) [[unlikely]] {
    return;
  }

  String modified(m_cap);

  const uint64_t candidates_amt = candidates.size();

  uint64_t i = 0;
  while (i < m_len) {
    bool replaced = false;

    for (uint64_t k = 0; k < candidates_amt; ++k) {
      if (impl_compare(candidates[k].GetU8Ptr(), candidates[k].get_length(), i)) {
        modified.append_slice(replacement);
        i += candidates[k].get_length();
        replaced = true;
        break;
      }
    }

    if (!replaced) {
      modified.append_byte(m_buf[i]);
      ++i;
    }
  }

  *this = std::move(modified);
}

bool String::starts_with_byte(uint8_t byte) const noexcept { return m_buf[0] == byte; }

bool String::starts_with_any_of_byte(StringSlice charset) const noexcept {
  return impl_is_any_of(m_buf[0], charset);
}

bool String::starts_with(StringSlice slice) const noexcept {
  return impl_compare(slice.GetU8Ptr(), slice.get_length(), 0);
}

bool String::starts_with_ignore_case(StringSlice slice) const noexcept {
  return impl_compare_ignore_case(slice.GetU8Ptr(), slice.get_length(), 0);
}

bool String::ends_with_byte(uint8_t byte) const noexcept {
  if (m_len == 0) [[unlikely]] {
    return false;
  }

  return m_buf[m_len - 1] == byte;
}

bool String::ends_with_any_of_bytes(StringSlice charset) const noexcept {
  if (m_len == 0) [[unlikely]] {
    return false;
  }

  return impl_is_any_of(m_buf[m_len - 1], charset);
}

void String::to_lower() noexcept {
  for (uint64_t i = 0; i < m_len; ++i) {
    m_buf[i] = cell::to_lower(m_buf[i]);
  }
}

void String::to_upper() noexcept {
  for (uint64_t i = 0; i < m_len; ++i) {
    m_buf[i] = cell::to_upper(m_buf[i]);
  }
}

void String::trim(uint8_t delimiter) noexcept {
  trim_right(delimiter);
  trim_left(delimiter);
}

void String::trim_left(uint8_t delimiter) noexcept {
  uint8_t *begin = m_buf;
  uint8_t *end = m_buf + m_len;

  while (*begin == delimiter) {
    ++begin;
  }

  const auto new_length = static_cast<uint64_t>(end - begin);

  memmove(m_buf, begin, new_length);
  mem_zero_ptr_range(m_buf + new_length, end);
  m_len = new_length;
}

void String::trim_right(uint8_t delimiter) noexcept {
  if (m_len == 0) {
    return;
  }

  uint8_t *end = m_buf + m_len - 1;  // minus one to not be on null-terminating character

  while (end >= m_buf && *end == delimiter) {
    --end;
  }

  mem_zero_ptr_range(end + 1, m_buf + m_len);
  m_len = static_cast<uint64_t>(end - m_buf) + 1;
}

// Clears contents and resets m_len
// Does not deallocate reserved memory
void String::clear() noexcept {
  mem_zero(m_buf, m_cap);
  m_len = 0;
}

void String::append_byte(uint8_t c) noexcept {
  if (m_len + 1 == m_cap) [[unlikely]] {
    expand(m_cap * 2);
  }

  m_buf[m_len] = c;
  m_buf[m_len + 1] = 0;
  ++m_len;
}

// before appending:
// *data_buffer_ = 0000000000000000 (m_cap = 16, m_len = 0)
//         ^
//         m_len
//
// example: appending "hello world" (strlen = 11)
//  after memcpy: *data_buffer_ = [hello world00000], m_len = 11
//                                    ^
//                                    m_len still points at 0, like it should
//
void String::append_c_str(const char *cstr) noexcept {
  const uint64_t l = string_length(cstr);

  if (l == 0) [[unlikely]] {
    return;
  }

  if (m_len + l >= m_cap) [[unlikely]] {
    expand(round_up_8(m_len + l + 1));
  }

  mem_copy(reinterpret_cast<uint8_t *>(m_buf + m_len), reinterpret_cast<const uint8_t *>(cstr), l);
  m_len += l;
  m_buf[m_len] = 0;
}

void String::append_slice(StringSlice slice) noexcept {
  const auto l = slice.get_length();

  if (m_len + l >= m_cap) [[unlikely]] {
    expand(round_up_8(m_len + l + 1));
  }

  memcpy(reinterpret_cast<uint8_t *>(m_buf + m_len), slice.GetU8Ptr(), l);
  m_len += l;
  m_buf[m_len] = 0;
}

void String::append_string(const String &other) noexcept {
  const uint64_t l = other.m_len;

  if (m_len + l >= m_cap) [[unlikely]] {
    expand(round_up_8(m_len + l + 1));
  }

  memcpy(reinterpret_cast<uint8_t *>(m_buf + m_len), other.m_buf, l);
  m_len += l;
  m_buf[m_len] = 0;
}

bool String::append_file_contents(const char *path) noexcept {
  constexpr int fd_error = -1;
  constexpr int fstat_error = -1;

  const int fd = ::open(path, O_RDONLY);
  if (fd == fd_error) {
    printf("%s: failed to open file '%s'\n", __PRETTY_FUNCTION__, path);
    return false;
  }

  struct stat statbuf;  // NOLINT
  if (fstat(fd, &statbuf) == fstat_error) {
    printf("%s: failed to stat file '%s'\n", __PRETTY_FUNCTION__, path);
    ::close(fd);
    return false;
  }

  const auto file_size = static_cast<uint64_t>(statbuf.st_size);
  expand(m_len + file_size + 1);

  const auto bytes_read = ::read(fd, m_buf + m_len, file_size);

  if (bytes_read == -1) {
    printf("%s: failed to read file '%s'\n", __PRETTY_FUNCTION__, path);
    ::close(fd);
    return false;
  }

  m_len += static_cast<uint64_t>(bytes_read);
  m_buf[m_len] = 0;
  ::close(fd);
  return true;
}

bool String::save_to_file(const char *path) const noexcept {
  constexpr int fd_error = -1;

  const int fd = ::open(path, O_WRONLY | O_CREAT, 00644);
  if (fd == fd_error) {
    printf("%s: failed to create file '%s'\n", __PRETTY_FUNCTION__, path);
    return false;
  }

  const auto bytes_written = ::write(fd, reinterpret_cast<const char *>(m_buf), m_len);

  if (bytes_written == -1) {
    printf("%s: failed to write to file '%s'\n", __PRETTY_FUNCTION__, path);
    ::close(fd);
    return false;
  }

  if (::fsync(fd) == -1) {
    printf("%s: failed to sync file '%s'\n", __PRETTY_FUNCTION__, path);
  }
  ::close(fd);
  return true;
}

void String::expand(uint64_t new_cap) {
  if (new_cap < m_cap) [[unlikely]] {
    return;
  }

  m_buf = mem_realloc<uint8_t>(m_buf, new_cap);
  //  mem_zero_ptr_range(data_buffer_ + m_len, data_buffer_ + new_cap);
  m_cap = new_cap;
}

void String::refresh_length() noexcept { m_len = string_length(reinterpret_cast<const char *>(m_buf)); }

const char *String::get_c_str_trimmed_left(uint8_t delimiter) const noexcept {
  uint8_t *begin = m_buf;

  while (*begin == delimiter) {
    ++begin;
  }

  return reinterpret_cast<const char *>(begin);
}

// -----------------------------------------------------------------------------
// Private Functions
// -----------------------------------------------------------------------------

bool String::impl_compare(const uint8_t *data, uint64_t length,
                     uint64_t offset) const noexcept {
  if (offset + length > m_len) {
    return false;
  }

  if (length == 0) [[unlikely]] {
    return true;
  }

  return mem_compare(data, m_buf + offset, length);
}

bool String::impl_compare_ignore_case(const uint8_t *data, uint64_t length,
                               uint64_t offset) const noexcept {
  if (offset + length > m_len) {
    return false;
  }

  if (length == 0) [[unlikely]] {
    return true;
  }

  for (uint64_t i = 0; i < length; ++i) {
    const uint8_t a{cell::to_lower(data[i])};
    const uint8_t b{cell::to_lower(m_buf[i + offset])};

    if (a != b) {
      return false;
    }
  }

  return true;
}

bool String::impl_is_any_of(uint8_t candidate, StringSlice charset) noexcept {
  uint8_t const *ptr = charset.GetU8Ptr();

  while (*ptr) {
    if (*ptr == candidate) {
      return true;
    }

    ++ptr;
  }

  return false;
}

StringSlice String::slice() const noexcept { return StringSlice{m_buf, m_len}; }

StringSlice String::slice(uint64_t from, uint64_t n) const noexcept {
  if (from + n <= m_len) [[likely]] {
    return {m_buf + from, n};
  }

  return {m_buf + m_len, 0};
}

StringSlice String::slice(uint64_t from) const noexcept {
  if (from < m_len) [[likely]] {
    return {m_buf + from, m_len - from};
  }

  return {m_buf + m_len, 0};
}

}  // namespace cell