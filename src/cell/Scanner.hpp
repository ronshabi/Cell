// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_SCANNER_HPP
#define CELL_SCANNER_HPP

#include <cstdint>

#include "cell/String.hpp"

namespace cell {

class Scanner {
 public:
  explicit Scanner(const String* string_ptr) noexcept;
  Scanner(Scanner&& other) noexcept;
  Scanner(const Scanner& other) = delete;
  Scanner& operator=(const Scanner& other) = delete;
  Scanner& operator=(Scanner&& other) = delete;

  static constexpr uint8_t kEOF = 0;

  [[nodiscard]] bool IsEof() const noexcept { return eof_; }

  // Get next char in stream until EOF, returns 0s after.
  // EOF also raises class' internal eof flag, which you
  // can get with IsEof()
  uint8_t GetNextChar() noexcept {
    if (cursor_ < string_->len_) [[likely]] {
      return string_->buf_[cursor_++];
    }

    eof_ = true;
    return kEOF;
  }

  [[nodiscard]] uint8_t Peek() const noexcept {
    if (cursor_ < string_->len_) [[likely]] {
      return string_->buf_[cursor_];
    }

    return kEOF;
  }

  void Advance() noexcept {
    if (cursor_ < string_->len_) [[likely]] {
      cursor_++;
    } else {
      eof_ = true;
    }
  }

  uint64_t AdvanceAnyOf(const char* charset) noexcept;
  uint64_t AdvanceUntilAnyOf(const char* charset) noexcept;

 private:
  uint64_t cursor_{0};
  const String* string_{nullptr};
  bool eof_{false};
};

}  // namespace cell

#endif  // CELL_SCANNER_HPP
