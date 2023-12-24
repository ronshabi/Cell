// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_CHARSET_HPP
#define CELL_CHARSET_HPP

#include "cell/core/Types.hpp"
#include "cell/log/Log.hpp"

namespace cell {

enum class Charset {
  Ascii,
  Utf8,
};

constexpr u8 kSP = 0x20;
constexpr u8 kLF = 0x0A;
constexpr u8 kCR = 0x0D;
constexpr u8 kHTAB = 0x09;

constexpr u8 kHexConversionFailure = 0xff;

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

[[nodiscard]] constexpr bool IsLower(const u8 byte) noexcept {
  return byte >= 0x61 && byte <= 0x7a;
}

[[nodiscard]] constexpr bool IsUpper(const u8 byte) noexcept {
  return byte >= 0x41 && byte <= 0x5a;
}

[[nodiscard]] constexpr bool IsAlpha(const u8 byte) noexcept {
  return IsLower(byte) || IsUpper(byte);
}

[[nodiscard]] constexpr bool IsDigit(const u8 byte) noexcept {
  return byte >= 0x30 && byte <= 0x39;
}

[[nodiscard]] constexpr bool IsHexUppercase(const u8 byte) noexcept {
  return IsDigit(byte) || (byte >= 0x41 && byte <= 0x46);
}

[[nodiscard]] constexpr bool IsHex(const u8 byte) noexcept {
  return IsHexUppercase(byte) || (byte >= 0x61 && byte <= 0x66);
}

[[nodiscard]] constexpr u8 HexDigitToByte(u8 dig) {
  if (dig >= 'A' && dig <= 'F') {
    return dig - 'A' + 10;
  } else if (dig >= 'a' && dig <= 'f') {
    return dig - 'a' + 10;
  } else if (dig >= '0' && dig <= '9') {
    return dig - '0';
  }

  return kHexConversionFailure; // not a hexdig
}

[[nodiscard]] constexpr u8 HexPairToByte(const u8 msb, const u8 lsb, bool& failed) noexcept {
  const auto msb_num = HexDigitToByte(msb);
  if (msb_num == kHexConversionFailure) {
    failed = true;
    return kHexConversionFailure;
  }

  const auto lsb_num = HexDigitToByte(lsb);
  if (lsb_num == kHexConversionFailure) {
    failed = true;
    return kHexConversionFailure;
  }

  return 16 * msb_num + lsb_num;
}

[[nodiscard]] constexpr bool IsWhitespace(const u8 byte) noexcept {
  return byte == kSP || byte == kHTAB || byte == kCR || byte == kLF | byte == '\v' ||
         byte == '\f' || byte == '\b';
}

[[nodiscard]] constexpr u8 ToLower(const u8 byte) noexcept {
  if (IsUpper(byte)) {
    return byte + 32;
  }

  return byte;
}

[[nodiscard]] constexpr u8 ToUpper(const u8 byte) noexcept {
  if (IsLower(byte)) {
    return byte - 32;
  }

  return byte;
}


namespace rfc9110 {
[[nodiscard]] constexpr bool IsWhitespace(const u8 byte) noexcept {
  return byte == kSP || byte == kHTAB;
}

[[nodiscard]] constexpr bool IsTChar(const u8 byte) noexcept {
  return IsAlpha(byte) || IsDigit(byte) || byte == '!' || byte == '#' || byte == '$' ||
         byte == '%' || byte == '\'' || byte == '_' || byte == '`' || byte == '|' || byte == '~';
}

[[nodiscard]] constexpr bool IsObsText(const u8 byte) noexcept {
  return byte >= 0x80 && byte <= 0xFF;
}
}

namespace rfc3986 {
[[nodiscard]] constexpr bool IsGenDelim(const u8 ch) noexcept {
  return ch == ':' || ch == '/' || ch == '?' || ch == '#' || ch == '[' || ch == ']' || ch == '@';
}

[[nodiscard]] constexpr bool IsSubDelim(const u8 ch) noexcept {
  return ch == '!' || ch == '$' || ch == '&' || ch == '\'' || ch == '(' || ch == ')' || ch == '*'
    || ch == '+' || ch == ',' || ch == ';' || ch == '=';
}

[[nodiscard]] constexpr bool IsReserved(const u8 ch) noexcept {
  return IsGenDelim(ch) || IsSubDelim(ch);
}

[[nodiscard]] constexpr bool IsUnreserved(const u8 ch) noexcept {
  return IsAlpha(ch) || IsDigit(ch) || ch == '-' || ch == '.' || ch == '_' || ch == '~';
}

// TODO
//[[nodiscard]] constexpr bool IsPctEncoded(const u8* ch) noexcept {
//  return IsAlpha(ch) || IsDigit(ch) || ch == '-' || ch == '.' || ch == '_' || ch == '~';
//}


//[[nodiscard]] constexpr bool IsPChar(const u8 ch) noexcept {
//  return IsUnreserved(ch) || IsPctEncoded(ch) || IsSubDelim(ch) || ch == ':' || ch == '@';
//}

}

}  // namespace cell

#endif  // CELL_CHARSET_HPP
