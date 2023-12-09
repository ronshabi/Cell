// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include "StringSlice.hpp"

#include <cstdint>

#include "Charset.hpp"
#include "String.hpp"
#include "cell/Memory.hpp"
#include "log/Log.hpp"

namespace cell {

StringSlice::StringSlice(const uint8_t *data)
    : StringSlice(data, Strlen(reinterpret_cast<const char *>(data))) {}

StringSlice::StringSlice(const uint8_t *data, uint64_t len) : data_(data), len_(len) {}

StringSlice StringSlice::FromCString(const char *data, uint64_t len) noexcept {
  return {reinterpret_cast<const uint8_t *>(data), len};
}

StringSlice StringSlice::FromCString(const char *data) noexcept {
  return StringSlice::FromCString(data, Strlen(data));
}

bool StringSlice::Compare(const StringSlice to) const noexcept {
  if (len_ != to.len_) [[likely]] {
    return false;
  }

  return MemCompare(data_, to.data_, len_);
}

bool StringSlice::CompareIgnoreCase(const StringSlice to) const noexcept {
  if (len_ != to.len_) [[likely]] {
    return false;
  }

  for (uint64_t i = 0; i < len_; ++i) {
    if (ToLower(data_[i]) != ToLower(to.data_[i])) {
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
