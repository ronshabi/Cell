// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_BASE_HPP
#define CELL_BASE_HPP

#include <cstdint>

#include "types.hpp"

namespace cell {

[[nodiscard]] constexpr u64 round_up_8(const u64 num) noexcept {
  if (num % 8 == 0) {
    return num;
  }

  return num - num % 8 + 8;
}

[[nodiscard]] constexpr u64 round_up_32(const u64 num) noexcept {
  if (num % 32 == 0) {
    return num;
  }

  return num - num % 32 + 32;
}


}  // namespace cell

#endif  // CELL_BASE_HPP
