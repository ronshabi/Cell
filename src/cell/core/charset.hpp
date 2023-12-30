// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_CHARSET_HPP
#define CELL_CHARSET_HPP

#include "cell/core/types.hpp"
#include "cell/log/Log.hpp"

namespace cell {

enum class Charset {
  Ascii,
  Utf8,
};

constexpr u8 SP = 0x20;
constexpr u8 LF = 0x0A;
constexpr u8 CR = 0x0D;
constexpr u8 HTAB = 0x09;

constexpr u8 CONVERSION_FAILURE = 0xff;

constexpr const char *ASCII_LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
constexpr const char *ASCII_UPPER = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
constexpr const char *ASCII_LOWER = "abcdefghijklmnopqrstuvwxyz";
constexpr const char *DIGITS = "0123456789";
constexpr const char *HEX_DIGITS_LOWER = "0123456789abcdef";
constexpr const char *HEX_DIGITS_UPPER = "0123456789ABCDEF";
constexpr const char *HEX_DIGITS = "0123456789abcdefABCDEF";
constexpr const char *OCTAL_DIGITS = "01234567";
constexpr const char *ASCII_PRINTABLE_STRICT =
    "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#$%&\'("
    ")*+,-./:;<=>?@[\\]^_`{|}~ \t\n";
constexpr const char *ASCII_PUNCT = R"(!"#$%&'()*+,-./:;<=>?@[\]^_`{|}~)";
constexpr const char *ASCII_WHITESPACE = " \n\t\r\f\v";

[[nodiscard]] constexpr bool is_lower(const u8 byte) noexcept {
  return byte >= 0x61 && byte <= 0x7a;
}

[[nodiscard]] constexpr bool is_upper(const u8 byte) noexcept {
  return byte >= 0x41 && byte <= 0x5a;
}

[[nodiscard]] constexpr bool is_alpha(const u8 byte) noexcept {
  return is_lower(byte) || is_upper(byte);
}

[[nodiscard]] constexpr bool is_digit(const u8 byte) noexcept {
  return byte >= 0x30 && byte <= 0x39;
}

[[nodiscard]] constexpr bool is_hex_upper(const u8 byte) noexcept {
  return is_digit(byte) || (byte >= 0x41 && byte <= 0x46);
}

[[nodiscard]] constexpr bool is_hex(const u8 byte) noexcept {
  return is_hex_upper(byte) || (byte >= 0x61 && byte <= 0x66);
}

[[nodiscard]] constexpr u8 hex_digits_to_byte(u8 dig) {
  if (dig >= 'A' && dig <= 'F') {
    return dig - 'A' + 10;
  } else if (dig >= 'a' && dig <= 'f') {
    return dig - 'a' + 10;
  } else if (dig >= '0' && dig <= '9') {
    return dig - '0';
  }

  return CONVERSION_FAILURE; // not a hexdig
}

[[nodiscard]] constexpr u8 hex_pair_to_byte(const u8 msb, const u8 lsb, bool& failed) noexcept {
  const auto msb_num = hex_digits_to_byte(msb);
  if (msb_num == CONVERSION_FAILURE) {
    failed = true;
    return CONVERSION_FAILURE;
  }

  const auto lsb_num = hex_digits_to_byte(lsb);
  if (lsb_num == CONVERSION_FAILURE) {
    failed = true;
    return CONVERSION_FAILURE;
  }

  return 16 * msb_num + lsb_num;
}

[[nodiscard]] constexpr bool is_whitespace(const u8 byte) noexcept {
  return byte == SP || byte == HTAB || byte == CR || byte == LF | byte == '\v' ||
         byte == '\f' || byte == '\b';
}

[[nodiscard]] constexpr u8 to_lower(const u8 byte) noexcept {
  if (is_upper(byte)) {
    return byte + 32;
  }

  return byte;
}

[[nodiscard]] constexpr u8 to_upper(const u8 byte) noexcept {
  if (is_lower(byte)) {
    return byte - 32;
  }

  return byte;
}


namespace rfc9110 {
[[nodiscard]] constexpr bool is_whitespace(const u8 byte) noexcept {
  return byte == SP || byte == HTAB;
}

[[nodiscard]] constexpr bool is_tchar(const u8 byte) noexcept {
  return is_alpha(byte) || is_digit(byte) || byte == '!' || byte == '#' || byte == '$' ||
         byte == '%' || byte == '\'' || byte == '_' || byte == '`' || byte == '|' || byte == '~';
}

[[nodiscard]] constexpr bool is_obstext(const u8 byte) noexcept {
  return byte >= 0x80 && byte <= 0xFF;
}
}

namespace rfc3986 {
[[nodiscard]] constexpr bool is_gendelim(const u8 ch) noexcept {
  return ch == ':' || ch == '/' || ch == '?' || ch == '#' || ch == '[' || ch == ']' || ch == '@';
}

[[nodiscard]] constexpr bool is_subdelim(const u8 ch) noexcept {
  return ch == '!' || ch == '$' || ch == '&' || ch == '\'' || ch == '(' || ch == ')' || ch == '*'
    || ch == '+' || ch == ',' || ch == ';' || ch == '=';
}

[[nodiscard]] constexpr bool is_reserved(const u8 ch) noexcept {
  return is_gendelim(ch) || is_subdelim(ch);
}

[[nodiscard]] constexpr bool is_unreserved(const u8 ch) noexcept {
  return is_alpha(ch) || is_digit(ch) || ch == '-' || ch == '.' || ch == '_' || ch == '~';
}

// TODO
//[[nodiscard]] constexpr bool IsPctEncoded(const u8* ch) noexcept {
//  return is_alpha(ch) || is_digit(ch) || ch == '-' || ch == '.' || ch == '_' || ch == '~';
//}


//[[nodiscard]] constexpr bool IsPChar(const u8 ch) noexcept {
//  return is_unreserved(ch) || IsPctEncoded(ch) || is_subdelim(ch) || ch == ':' || ch == '@';
//}

}

}  // namespace cell

#endif  // CELL_CHARSET_HPP
