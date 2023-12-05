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

#endif  // CELL_BASE_HPP
