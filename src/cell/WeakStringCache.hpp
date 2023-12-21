// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_WEAKSTRINGCACHE_HPP
#define CELL_WEAKSTRINGCACHE_HPP

#include <cstdint>
#include <vector>

#include "String.hpp"
#include "StringSlice.hpp"

namespace cell {

class WeakStringCache {
 public:
  static constexpr uint64_t kKeyDoesNotExist = static_cast<uint64_t>(-1);

  explicit WeakStringCache() noexcept = default;

  uint64_t AddKeyValuePair(const String& k, const String& v) noexcept;
  uint64_t AppendToValue(StringSlice k, StringSlice v) noexcept;
  [[nodiscard]] uint64_t SearchKey(StringSlice k) const noexcept;
  [[nodiscard]] uint64_t SearchKeyIgnoreCase(StringSlice k) const noexcept;
  [[nodiscard]] StringSlice GetKeyAtIndex(uint64_t index) const noexcept;

 private:
  std::vector<std::pair<String, String>> table_;
};

}  // namespace cell

#endif  // CELL_WEAKSTRINGCACHE_HPP
