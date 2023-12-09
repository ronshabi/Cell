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
  Scanner& operator=(Scanner&& other) noexcept;

  static constexpr uint8_t kEOF = 0;

  [[nodiscard]] bool IsEof() const noexcept { return eof_; }
  [[nodiscard]] uint8_t Peek() const noexcept;
  [[nodiscard]] uint8_t GetNextChar() noexcept;
  void Advance(uint64_t len = 1) noexcept;

  void ResetState() noexcept;
  void AppendToBufferUntilHittingChar(String& outbuffer, uint8_t ch) noexcept;
  bool AdvanceContinuousExactly(uint64_t amount, const uint8_t ch) noexcept;
  //  uint64_t AdvanceAnyOf(StringSlice charset) noexcept;
  //  uint64_t AdvanceUntilAnyOf(const char* charset) noexcept;
  //  [[nodiscard]] uint8_t GetNextCharUntilAnyOf(const char* charset) noexcept;
  //  [[nodiscard]] bool CompareWordAtCursor(const char* to) const noexcept;
  //  [[nodiscard]] bool CompareWordAtCursorIgnoreCase(const char* to) const noexcept;

 private:
  uint64_t cursor_{0};
  const String* string_{nullptr};
  bool eof_{false};
};

}  // namespace cell

#endif  // CELL_SCANNER_HPP
