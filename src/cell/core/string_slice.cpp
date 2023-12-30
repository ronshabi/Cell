// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include "string_slice.hpp"

#include <cstdint>

#include "cell/log/log.hpp"
#include "charset.hpp"
#include "memory.hpp"
#include "string.hpp"

namespace cell {

StringSlice::StringSlice(const uint8_t *data)
    : StringSlice(data, string_length(reinterpret_cast<const char *>(data))) {}

StringSlice::StringSlice(const uint8_t *data, uint64_t len) : m_data(data), m_len(len) {}

StringSlice StringSlice::from_cstr(const char *data, uint64_t len) noexcept {
  return {reinterpret_cast<const uint8_t *>(data), len};
}

StringSlice StringSlice::from_cstr(const char *data) noexcept {
  return StringSlice::from_cstr(data, string_length(data));
}

bool StringSlice::compare(StringSlice to) const noexcept {
  if (m_len != to.m_len) [[likely]] {
    return false;
  }

  return mem_compare(m_data, to.m_data, m_len);
}

bool StringSlice::compare_ignore_case(StringSlice to) const noexcept {
  if (m_len != to.m_len) [[likely]] {
    return false;
  }

  for (uint64_t i = 0; i < m_len; ++i) {
    if (to_lower(m_data[i]) != to_lower(to.m_data[i])) {
      return false;
    }
  }

  return true;
}
bool StringSlice::contains(uint8_t byte) const noexcept {
  for (uint64_t i = 0; i < m_len; ++i) {
    if (m_data[i] == byte) {
      return true;
    }
  }

  return false;
}
}  // namespace cell
