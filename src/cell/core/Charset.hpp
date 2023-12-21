// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_CHARSET_HPP
#define CELL_CHARSET_HPP

#include <cstdint>

namespace cell {

enum class Charset {
  Ascii,
  Utf8,
};

constexpr uint8_t kSP = 0x20;
constexpr uint8_t kLF = 0x0A;
constexpr uint8_t kCR = 0x0D;
constexpr uint8_t kHTAB = 0x09;

constexpr const char *kAsciiLetters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
constexpr const char *kAsciiUpper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
constexpr const char *kAsciiLower = "abcdefghijklmnopqrstuvwxyz";
constexpr const char *kDigits = "0123456789";
constexpr const char *kHexDigitsLower = "0123456789abcdef";
constexpr const char *kHexDigitsUpper = "0123456789ABCDEF";
constexpr const char *kHexDigits = "0123456789abcdefABCDEF";
constexpr const char *kOctalDigits = "01234567";
constexpr const char *kAsciiPrintableStrict =
    "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#$%&\'("
    ")*+,-./:;<=>?@[\\]^_`{|}~ \t\n";
constexpr const char *kAsciiPunct = R"(!"#$%&'()*+,-./:;<=>?@[\]^_`{|}~)";
constexpr const char *kAsciiWhitespace = " \n\t\r\f\v";

[[nodiscard]] constexpr bool IsLower(const uint8_t byte) noexcept {
  return byte >= 0x61 && byte <= 0x7a;
}

[[nodiscard]] constexpr bool IsUpper(const uint8_t byte) noexcept {
  return byte >= 0x41 && byte <= 0x5a;
}

[[nodiscard]] constexpr bool IsAlpha(const uint8_t byte) noexcept {
  return IsLower(byte) || IsUpper(byte);
}

[[nodiscard]] constexpr bool IsDigit(const uint8_t byte) noexcept {
  return byte >= 0x30 && byte <= 0x39;
}

[[nodiscard]] constexpr bool IsHexUppercase(const uint8_t byte) noexcept {
  return IsDigit(byte) || (byte >= 0x41 && byte <= 0x46);
}

[[nodiscard]] constexpr bool IsHex(const uint8_t byte) noexcept {
  return IsHexUppercase(byte) || (byte >= 0x61 && byte <= 0x66);
}

[[nodiscard]] constexpr bool IsWhitespace(const uint8_t byte) noexcept {
  return byte == kSP || byte == kHTAB || byte == kCR || byte == kLF | byte == '\v' ||
         byte == '\f' || byte == '\b';
}

[[nodiscard]] constexpr bool Rfc9110_IsWhitespace(const uint8_t byte) noexcept {
  return byte == kSP || byte == kHTAB;
}

[[nodiscard]] constexpr bool Rfc9110_IsTChar(const uint8_t byte) noexcept {
  return IsAlpha(byte) || IsDigit(byte) || byte == '!' || byte == '#' || byte == '$' ||
         byte == '%' || byte == '\'' || byte == '_' || byte == '`' || byte == '|' || byte == '~';
}

[[nodiscard]] constexpr bool Rfc9110_IsObsText(const uint8_t byte) noexcept {
  return byte >= 0x80 && byte <= 0xFF;
}

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
