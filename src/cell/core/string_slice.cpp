// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include "string_slice.hpp"

#include <cstdint>

#include "cell/log/Log.hpp"
#include "charset.hpp"
#include "memory.hpp"
#include "string.hpp"

namespace cell {

StringSlice::StringSlice(const uint8_t *data)
    : StringSlice(data, string_length(reinterpret_cast<const char *>(data))) {}

StringSlice::StringSlice(const uint8_t *data, uint64_t len) : data_(data), len_(len) {}

StringSlice StringSlice::FromCString(const char *data, uint64_t len) noexcept {
  return {reinterpret_cast<const uint8_t *>(data), len};
}

StringSlice StringSlice::from_cstr(const char *data) noexcept {
  return StringSlice::FromCString(data, string_length(data));
}

bool StringSlice::Compare(const StringSlice to) const noexcept {
  if (len_ != to.len_) [[likely]] {
    return false;
  }

  return mem_compare(data_, to.data_, len_);
}

bool StringSlice::CompareIgnoreCase(const StringSlice to) const noexcept {
  if (len_ != to.len_) [[likely]] {
    return false;
  }

  for (uint64_t i = 0; i < len_; ++i) {
    if (to_lower(data_[i]) != to_lower(to.data_[i])) {
      return false;
    }
  }

  return true;
}
bool StringSlice::Contains(uint8_t byte) const noexcept {
  for (uint64_t i = 0; i < len_; ++i) {
    if (data_[i] == byte) {
      return true;
    }
  }

  return false;
}
}  // namespace cell
