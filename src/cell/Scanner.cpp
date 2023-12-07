// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include "Scanner.hpp"

namespace cell {

Scanner::Scanner(const String *string_ptr) noexcept : string_(string_ptr) {}

Scanner::Scanner(Scanner &&other) noexcept : cursor_(other.cursor_), string_(other.string_) {
  other.cursor_ = 0;
  other.string_ = nullptr;
}

uint64_t Scanner::AdvanceAnyOf(const char *charset) noexcept {
  uint64_t cnt = 0;

  while (!eof_ && String::isAnyOf(Peek(), charset)) {
    Advance();
    ++cnt;
  }

  return cnt;
}

uint64_t Scanner::AdvanceUntilAnyOf(const char *charset) noexcept {
  uint64_t cnt = 0;

  while (!eof_ && !String::isAnyOf(Peek(), charset)) {
    Advance();
    ++cnt;
  }

  return cnt;
}

}  // namespace cell