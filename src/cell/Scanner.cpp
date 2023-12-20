// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include "Scanner.hpp"

#include "Memory.hpp"
#include "log/Log.hpp"

namespace cell {

Scanner::Scanner(const String *string_ptr) noexcept : string_(string_ptr) {}

Scanner::Scanner(Scanner &&other) noexcept : cursor_(other.cursor_), string_(other.string_) {
  other.cursor_ = 0;
  other.string_ = nullptr;
}

uint8_t Scanner::Peek() const noexcept {
  if (cursor_ < string_->len_) [[likely]] {
    return string_->buf_[cursor_];
  }

  return kEOF;
}

// Get next char in stream until EOF, returns 0s after.
// EOF also raises class' internal eof flag, which you
// can get with IsEof()
uint8_t Scanner::GetNextChar() noexcept {
  if (cursor_ < string_->len_) [[likely]] {
    return string_->buf_[cursor_++];
  }

  eof_ = true;
  return kEOF;
}

Scanner &Scanner::operator=(Scanner &&other) noexcept {
  cursor_ = other.cursor_;
  string_ = other.string_;
  eof_ = other.eof_;

  other.string_ = nullptr;
  return *this;
}

void Scanner::Advance(const uint64_t len) noexcept {
  if (cursor_ + len < string_->len_) [[likely]] {
    cursor_ += len;
  } else {
    eof_ = true;
  }
}

void Scanner::ResetState() noexcept {
  cursor_ = 0;
  eof_ = false;
}

void Scanner::AppendToBufferUntilHittingChar(String &outbuffer, uint8_t ch) noexcept {
  while (true) {
    const uint8_t current = GetNextChar();

    if (eof_ || current == ch) {
      break;
    }

    outbuffer.AppendChar(current);
    //    CELL_LOG_DEBUG("Appending [%c]", current);
  }
}

bool Scanner::AdvanceContinuousExactly(const uint8_t ch, uint64_t amount) noexcept {
  CELL_ASSERT(amount != 0);

  while (true) {
    const uint8_t current = GetNextChar();
    CELL_LOG_DEBUG("Cursor at (%lu/%lu) [0x%X]. Target: 0x%X Amount left: %lu, EOF: %d", cursor_,
                   string_->len_, ch, current, amount, eof_);
    if (!eof_ && current == ch) {
      CELL_LOG_DEBUG("Found [0x%X]", current);
      --amount;
    }

    if (eof_) {
      return amount == 0;
    }

    if (amount == 0) {
      return true;
    }

    return false;
  }
}

//
// uint64_t Scanner::AdvanceSpace() noexcept {
//  uint64_t cnt = 0;
//
//  while (!eof_ && Peek() == ' ') {
//    Advance();
//    ++cnt;
//  }
//
//  return cnt;
//}

//
// uint64_t Scanner::AdvanceAnyOf(const StringSlice charset) noexcept {
//  uint64_t cnt = 0;
//
//  while (!eof_ && String::isAnyOf(Peek(), charset)) {
//    Advance();
//    ++cnt;
//  }
//
//  return cnt;
//}
//
// uint64_t Scanner::AdvanceUntilAnyOf(const char *charset) noexcept {
//  uint64_t cnt = 0;
//
//  while (!eof_ && !String::isAnyOf(Peek(), charset)) {
//    Advance();
//    ++cnt;
//  }
//
//  return cnt;
//}
//
// uint8_t Scanner::GetNextCharUntilAnyOf(const char *charset) noexcept {
//  if (cursor_ < string_->len_ && String::isAnyOf(Peek(), charset)) [[likely]] {
//    return string_->data_buffer_[cursor_++];
//  }
//
//  eof_ = true;
//  return kEOF;
//}
//
// bool Scanner::CompareWordAtCursor(const char *to) const noexcept {
//  return string_->compare(to, Strlen(to), cursor_);
//}
//
// bool Scanner::CompareWordAtCursorIgnoreCase(const char *to) const noexcept {
//  return string_->compareIgnoreCase(to, Strlen(to), cursor_);
//}

}  // namespace cell