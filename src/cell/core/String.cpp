// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include "String.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdint>

#include "Assert.hpp"
#include "Base.hpp"
#include "Charset.hpp"
#include "Memory.hpp"
#include "StringSlice.hpp"
#include "WeakStringCache.hpp"

namespace cell {

// -----------------------------------------------------------------------------
// Constructors/Destructors
// -----------------------------------------------------------------------------

String::String() noexcept : String(8) {}

String::String(uint64_t initial_capacity_hint) noexcept
    : cap_(RoundUp8(initial_capacity_hint)), len_(0), buf_(Malloc<uint8_t>(cap_)) {
  MemZero(buf_, cap_);
}

String::String(StringSlice slice) noexcept
    : cap_(RoundUp8(slice.GetLen())), len_(slice.GetLen()), buf_(Malloc<uint8_t>(slice.GetLen())) {
  MemZero(buf_, cap_);
  MemCopy(buf_, slice.GetU8Ptr(), len_);
}

String::String(const String &other) noexcept
    : cap_(other.cap_), len_(other.len_), buf_(Malloc<uint8_t>(other.cap_)) {
  MemCopy(buf_, other.buf_, other.cap_);
}

String::String(String &&other) noexcept : cap_(other.cap_), len_(other.len_), buf_(other.buf_) {
  CELL_ASSERT(other.buf_ != nullptr);
  other.buf_ = nullptr;
}

String::~String() { Free(buf_); }

// -----------------------------------------------------------------------------
// Assignment Operators
// -----------------------------------------------------------------------------

String &String::operator=(const String &other) noexcept {
  Grow(other.cap_);
  len_ = other.len_;
  MemCopy(buf_, other.buf_, other.len_);

  return *this;
}

String &String::operator=(String &&other) noexcept {
  CELL_ASSERT(other.buf_ != nullptr);
  MemZero(buf_, cap_);
  Free(buf_);
  cap_ = other.cap_;
  len_ = other.len_;
  buf_ = other.buf_;
  other.buf_ = nullptr;

  return *this;
}

// -----------------------------------------------------------------------------
// Public Functions
// -----------------------------------------------------------------------------

bool String::Compare(const StringSlice slice) const noexcept {
  return len_ == slice.GetLen() && compare(slice.GetU8Ptr(), len_, 0);
}

bool String::CompareIgnoreCase(const StringSlice slice) const noexcept {
  return len_ == slice.GetLen() && compareIgnoreCase(slice.GetU8Ptr(), len_, 0);
}

bool String::Contains(uint8_t byte) const noexcept {
  for (uint64_t i = 0; i < len_; i++) {
    if (buf_[i] == byte) {
      return true;
    }
  }

  return false;
}

bool String::Contains(const StringSlice slice) const noexcept {
  for (uint64_t i = 0; i < len_; i++) {
    if (compare(slice.GetU8Ptr(), slice.GetLen(), i)) {
      return true;
    }
  }

  return false;
}

bool String::ContainsIgnoreCase(const StringSlice slice) const noexcept {
  for (uint64_t i = 0; i < len_; i++) {
    if (compareIgnoreCase(slice.GetU8Ptr(), slice.GetLen(), i)) {
      return true;
    }
  }

  return false;
}

bool String::ContainsAnyOf(const StringSlice charset) const noexcept {
  for (uint64_t i = 0; i < len_; i++) {
    if (isAnyOf(buf_[i], charset)) {
      return true;
    }
  }

  return false;
}

bool String::ContainsAnyOf(const std::unordered_set<uint8_t> &charset) const noexcept {
  for (uint64_t i = 0; i < len_; i++) {
    if (charset.contains(buf_[i])) {
      return true;
    }
  }

  return false;
}

bool String::ContainsJust(const StringSlice charset) const noexcept {
  for (uint64_t i = 0; i < len_; i++) {
    if (!isAnyOf(buf_[i], charset)) {
      return false;
    }
  }

  return true;
}

bool String::ContainsJust(const std::unordered_set<uint8_t> &charset) const noexcept {
  for (uint64_t i = 0; i < len_; i++) {
    if (!charset.contains(buf_[i])) {
      return false;
    }
  }

  return true;
}

void String::ReplaceChar(const uint8_t original, const uint8_t replacement) noexcept {
  for (uint64_t i = 0; i < len_; i++) {
    if (buf_[i] == original) {
      buf_[i] = replacement;
    }
  }
}

void String::ReplaceAnyOfChars(const StringSlice charset, const uint8_t replacement) noexcept {
  for (uint64_t i = 0; i < len_; i++) {
    if (isAnyOf(buf_[i], charset)) {
      buf_[i] = replacement;
    }
  }
}

void String::Replace(const StringSlice candidate, const StringSlice replacement) noexcept {
  String modified(cap_);

  uint64_t i = 0;
  while (i < len_) {
    if (compare(candidate.GetU8Ptr(), candidate.GetLen(), i)) {
      modified.AppendStringSlice(replacement);
      i += candidate.GetLen();
    } else {
      modified.AppendByte(buf_[i]);
      ++i;
    }
  }

  *this = std::move(modified);
}

void String::ReplaceAnyOf(const std::vector<StringSlice> &candidates,
                          const StringSlice replacement) noexcept {
  if (candidates.empty()) [[unlikely]] {
    return;
  }

  String modified(cap_);

  const uint64_t candidates_amt = candidates.size();

  uint64_t i = 0;
  while (i < len_) {
    bool replaced = false;

    for (uint64_t k = 0; k < candidates_amt; ++k) {
      if (compare(candidates[k].GetU8Ptr(), candidates[k].GetLen(), i)) {
        modified.AppendStringSlice(replacement);
        i += candidates[k].GetLen();
        replaced = true;
        break;
      }
    }

    if (!replaced) {
      modified.AppendByte(buf_[i]);
      ++i;
    }
  }

  *this = std::move(modified);
}

bool String::StartsWithChar(const uint8_t byte) const noexcept { return buf_[0] == byte; }

bool String::StartsWithAnyOfChars(const StringSlice charset) const noexcept {
  return isAnyOf(buf_[0], charset);
}

bool String::StartsWith(const StringSlice slice) const noexcept {
  return compare(slice.GetU8Ptr(), slice.GetLen(), 0);
}

bool String::StartsWithIgnoreCase(const StringSlice slice) const noexcept {
  return compareIgnoreCase(slice.GetU8Ptr(), slice.GetLen(), 0);
}

bool String::EndsWithChar(const uint8_t byte) const noexcept {
  if (len_ == 0) [[unlikely]] {
    return false;
  }

  return buf_[len_ - 1] == byte;
}

bool String::EndsWithAnyOfChars(const StringSlice charset) const noexcept {
  if (len_ == 0) [[unlikely]] {
    return false;
  }

  return isAnyOf(buf_[len_ - 1], charset);
}

void String::ToLower() noexcept {
  for (uint64_t i = 0; i < len_; ++i) {
    buf_[i] = cell::ToLower(buf_[i]);
  }
}

void String::ToUpper() noexcept {
  for (uint64_t i = 0; i < len_; ++i) {
    buf_[i] = cell::ToUpper(buf_[i]);
  }
}

void String::Trim(const uint8_t delimiter) noexcept {
  TrimRight(delimiter);
  TrimLeft(delimiter);
}

void String::TrimLeft(const uint8_t delimiter) noexcept {
  uint8_t *begin = buf_;
  uint8_t *end = buf_ + len_;

  while (*begin == delimiter) {
    ++begin;
  }

  const auto new_length = static_cast<uint64_t>(end - begin);

  memmove(buf_, begin, new_length);
  MemZeroPtrRange(buf_ + new_length, end);
  len_ = new_length;
}

void String::TrimRight(const uint8_t delimiter) noexcept {
  if (len_ == 0) {
    return;
  }

  uint8_t *end = buf_ + len_ - 1;  // minus one to not be on null-terminating character

  while (end >= buf_ && *end == delimiter) {
    --end;
  }

  MemZeroPtrRange(end + 1, buf_ + len_);
  len_ = static_cast<uint64_t>(end - buf_) + 1;
}

// Clears contents and resets len_
// Does not deallocate reserved memory
void String::Clear() noexcept {
  MemZero(buf_, cap_);
  len_ = 0;
}

void String::AppendByte(uint8_t c) noexcept {
  if (len_ + 1 == cap_) [[unlikely]] {
    Grow(cap_ * 2);
  }

  buf_[len_] = c;
  buf_[len_ + 1] = 0;
  ++len_;
}

// before appending:
// *data_buffer_ = 0000000000000000 (cap_ = 16, len_ = 0)
//         ^
//         len_
//
// example: appending "hello world" (strlen = 11)
//  after memcpy: *data_buffer_ = [hello world00000], len_ = 11
//                                    ^
//                                    len_ still points at 0, like it should
//
void String::AppendCString(const char *cstr) noexcept {
  const uint64_t l = Strlen(cstr);

  if (l == 0) [[unlikely]] {
    return;
  }

  if (len_ + l >= cap_) [[unlikely]] {
    Grow(RoundUp8(len_ + l + 1));
  }

  MemCopy(reinterpret_cast<uint8_t *>(buf_ + len_), reinterpret_cast<const uint8_t *>(cstr), l);
  len_ += l;
  buf_[len_] = 0;
}

void String::AppendStringSlice(const StringSlice slice) noexcept {
  const auto l = slice.GetLen();

  if (len_ + l >= cap_) [[unlikely]] {
    Grow(RoundUp8(len_ + l + 1));
  }

  memcpy(reinterpret_cast<uint8_t *>(buf_ + len_), slice.GetU8Ptr(), l);
  len_ += l;
  buf_[len_] = 0;
}

void String::AppendString(const String &other) noexcept {
  const uint64_t l = other.len_;

  if (len_ + l >= cap_) [[unlikely]] {
    Grow(RoundUp8(len_ + l + 1));
  }

  memcpy(reinterpret_cast<uint8_t *>(buf_ + len_), other.buf_, l);
  len_ += l;
  buf_[len_] = 0;
}

bool String::AppendFileContents(const char *path) noexcept {
  constexpr int fd_error = -1;
  constexpr int fstat_error = -1;

  const int fd = ::open(path, O_RDONLY);
  if (fd == fd_error) {
    printf("%s: failed to open file '%s'\n", __PRETTY_FUNCTION__, path);
    return false;
  }

  struct stat statbuf;  // NOLINT
  if (fstat(fd, &statbuf) == fstat_error) {
    printf("%s: failed to stat file '%s'\n", __PRETTY_FUNCTION__, path);
    ::close(fd);
    return false;
  }

  const auto file_size = static_cast<uint64_t>(statbuf.st_size);
  Grow(len_ + file_size + 1);

  const auto bytes_read = ::read(fd, buf_ + len_, file_size);

  if (bytes_read == -1) {
    printf("%s: failed to read file '%s'\n", __PRETTY_FUNCTION__, path);
    ::close(fd);
    return false;
  }

  len_ += static_cast<uint64_t>(bytes_read);
  buf_[len_] = 0;
  ::close(fd);
  return true;
}

bool String::SaveToFile(const char *path) const noexcept {
  constexpr int fd_error = -1;

  const int fd = ::open(path, O_WRONLY | O_CREAT, 00644);
  if (fd == fd_error) {
    printf("%s: failed to create file '%s'\n", __PRETTY_FUNCTION__, path);
    return false;
  }

  const auto bytes_written = ::write(fd, reinterpret_cast<const char *>(buf_), len_);

  if (bytes_written == -1) {
    printf("%s: failed to write to file '%s'\n", __PRETTY_FUNCTION__, path);
    ::close(fd);
    return false;
  }

  if (::fsync(fd) == -1) {
    printf("%s: failed to sync file '%s'\n", __PRETTY_FUNCTION__, path);
  }
  ::close(fd);
  return true;
}

void String::Grow(const uint64_t new_cap) {
  if (new_cap < cap_) [[unlikely]] {
    return;
  }

  buf_ = Realloc<uint8_t>(buf_, new_cap);
  //  MemZeroPtrRange(data_buffer_ + len_, data_buffer_ + new_cap);
  cap_ = new_cap;
}

void String::RefreshLength() noexcept { len_ = Strlen(reinterpret_cast<const char *>(buf_)); }

const char *String::GetCStringTrimmedLeft(const uint8_t delimiter) const noexcept {
  uint8_t *begin = buf_;

  while (*begin == delimiter) {
    ++begin;
  }

  return reinterpret_cast<const char *>(begin);
}

// -----------------------------------------------------------------------------
// Private Functions
// -----------------------------------------------------------------------------

bool String::compare(const uint8_t *data, const uint64_t length,
                     const uint64_t offset) const noexcept {
  if (offset + length > len_) {
    return false;
  }

  if (length == 0) [[unlikely]] {
    return true;
  }

  return MemCompare(data, buf_ + offset, length);
}

bool String::compareIgnoreCase(const uint8_t *data, const uint64_t length,
                               const uint64_t offset) const noexcept {
  if (offset + length > len_) {
    return false;
  }

  if (length == 0) [[unlikely]] {
    return true;
  }

  for (uint64_t i = 0; i < length; ++i) {
    const uint8_t a{cell::ToLower(data[i])};
    const uint8_t b{cell::ToLower(buf_[i + offset])};

    if (a != b) {
      return false;
    }
  }

  return true;
}

bool String::isAnyOf(const uint8_t candidate, const StringSlice charset) noexcept {
  uint8_t const *ptr = charset.GetU8Ptr();

  while (*ptr) {
    if (*ptr == candidate) {
      return true;
    }

    ++ptr;
  }

  return false;
}

StringSlice String::SubSlice() const noexcept { return StringSlice{buf_, len_}; }

StringSlice String::SubSlice(uint64_t from, uint64_t n) const noexcept {
  if (from + n <= len_) [[likely]] {
    return {buf_ + from, n};
  }

  return {buf_ + len_, 0};
}

StringSlice String::SubSlice(uint64_t from) const noexcept {
  if (from < len_) [[likely]] {
    return {buf_ + from, len_ - from};
  }

  return {buf_ + len_, 0};
}

}  // namespace cell