// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_CHARSET_HPP
#define CELL_CHARSET_HPP

#include <cstdint>

namespace cell {

[[nodiscard]] constexpr bool IsLower(const uint8_t byte) noexcept { return byte >= 0x61 && byte <= 0x7a; }

[[nodiscard]] constexpr bool IsUpper(const uint8_t byte) noexcept { return byte >= 0x41 && byte <= 0x5a; }

[[nodiscard]] constexpr uint8_t ToLower(const uint8_t byte) noexcept {
  if (IsUpper(byte)) {
    return byte + 32;
  }

  return byte;
}

[[nodiscard]] constexpr uint8_t ToUpper(const uint8_t byte) noexcept {
  if (IsLower(byte)) {
    return byte - 32;
  }

  return byte;
}

}  // namespace cell

#endif  // CELL_CHARSET_HPP
