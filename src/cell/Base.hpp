// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_BASE_HPP
#define CELL_BASE_HPP

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "cell/Assert.hpp"

namespace cell {

[[nodiscard]] constexpr uint64_t RoundUp8(const uint64_t num) noexcept {
  if (num % 8 == 0) {
    return num;
  }

  return num - num % 8 + 8;
}

[[nodiscard]] constexpr uint64_t RoundUp32(const uint64_t num) noexcept {
  if (num % 32 == 0) {
    return num;
  }

  return num - num % 32 + 32;
}

}  // namespace cell

#endif  // CELL_BASE_HPP
