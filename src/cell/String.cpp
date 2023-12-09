// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include "String.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "Charset.hpp"
#include "cell/Assert.hpp"
#include "cell/Base.hpp"
#include "cell/Memory.hpp"

namespace cell {

// -----------------------------------------------------------------------------
// Constructors/Destructors
// -----------------------------------------------------------------------------

String::String() noexcept : String(8) {}

String::String(uint64_t initial_capacity_hint) noexcept
    : cap_(RoundUp8(initial_capacity_hint)), len_(0), buf_(Malloc<uint8_t>(cap_)) {
  MemZero(buf_, cap_);
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

bool String::Compare(const char *cstr) const noexcept {
  return len_ == Strlen(cstr) && compare(cstr, len_, 0);
}

bool String::Compare(std::string_view str) const noexcept {
  return len_ == str.size() && compare(str.data(), len_, 0);
}

bool String::CompareIgnoreCase(const char *cstr) const noexcept {
  return len_ == Strlen(cstr) && compareIgnoreCase(cstr, len_, 0);
}

bool String::CompareIgnoreCase(std::string_view sv) const noexcept {
  return len_ == sv.size() && compareIgnoreCase(sv.data(), len_, 0);
}

bool String::Contains(uint8_t byte) const noexcept {
  for (uint64_t i = 0; i < len_; i++) {
    if (buf_[i] == byte) {
      return true;
    }
  }

  return false;
}

bool String::Contains(const char *cstr) const noexcept {
  const uint64_t l = Strlen(cstr);

  for (uint64_t i = 0; i < len_; i++) {
    if (compare(cstr, l, i)) {
      return true;
    }
  }

  return false;
}

bool String::Contains(const std::string_view sv) const noexcept {
  for (uint64_t i = 0; i < len_; i++) {
    if (compare(sv.data(), sv.size(), i)) {
      return true;
    }
  }

  return false;
}

bool String::ContainsIgnoreCase(const char *cstr) const noexcept {
  const uint64_t l = Strlen(cstr);

  for (uint64_t i = 0; i < len_; i++) {
    if (compareIgnoreCase(cstr, l, i)) {
      return true;
    }
  }

  return false;
}

bool String::ContainsIgnoreCase(const std::string_view sv) const noexcept {
  for (uint64_t i = 0; i < len_; i++) {
    if (compareIgnoreCase(sv.data(), sv.size(), i)) {
      return true;
    }
  }

  return false;
}

bool String::ContainsAnyOf(const char *charset) const noexcept {
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

bool String::ContainsJust(const char *charset) const noexcept {
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

void String::ReplaceAnyOfChars(const char *charset, const uint8_t replacement) noexcept {
  for (uint64_t i = 0; i < len_; i++) {
    if (isAnyOf(buf_[i], charset)) {
      buf_[i] = replacement;
    }
  }
}

void String::Replace(const char *candidate, const char *replacement) noexcept {
  String modified(cap_);
  const uint64_t candidate_length = Strlen(candidate);

  uint64_t i = 0;
  while (i < len_) {
    if (compare(candidate, candidate_length, i)) {
      modified.AppendCString(replacement);
      i += candidate_length;
    } else {
      modified.AppendChar(buf_[i]);
      ++i;
    }
  }

  *this = std::move(modified);
}

void String::ReplaceAnyOf(const std::vector<const char *> &candidates,
                          const char *replacement) noexcept {
  if (candidates.empty()) [[unlikely]] {
    return;
  }

  String modified(cap_);

  const uint64_t candidates_amt = candidates.size();
  std::vector<uint64_t> candidates_lengths;
  candidates_lengths.reserve(candidates_amt);

  for (const char *candidate : candidates) {
    candidates_lengths.push_back(Strlen(candidate));
  }

  uint64_t i = 0;
  while (i < len_) {
    bool replaced = false;

    for (uint64_t k = 0; k < candidates_amt; ++k) {
      if (compare(candidates[k], candidates_lengths[k], i)) {
        modified.AppendCString(replacement);
        i += candidates_lengths[k];
        replaced = true;
        break;
      }
    }

    if (!replaced) {
      modified.AppendChar(buf_[i]);
      ++i;
    }
  }

  *this = std::move(modified);
}

void String::ReplaceHashmap(
    const std::unordered_map<const char *, const char *> &hashmap) noexcept {
  String modified(cap_);
  std::unordered_map<const char *, uint64_t> lengths;

  for (const auto [k, v] : hashmap) {
    lengths[k] = Strlen(k);
  }

  uint64_t i = 0;
  while (i < len_) {
    for (const auto [k, v] : hashmap) {
      if (compare(k, lengths[k], i)) {
        modified.AppendCString(v);
        i += lengths[k];
        break;
      }
    }

    modified.AppendChar(buf_[i]);
    ++i;
  }

  *this = std::move(modified);
}

bool String::StartsWithChar(const uint8_t byte) const noexcept { return buf_[0] == byte; }

bool String::StartsWithAnyOfChars(const char *charset) const noexcept {
  return isAnyOf(buf_[0], charset);
}

bool String::StartsWith(const char *cstr) const noexcept { return compare(cstr, Strlen(cstr), 0); }

bool String::StartsWithIgnoreCase(const char *cstr) const noexcept {
  return compareIgnoreCase(cstr, Strlen(cstr), 0);
}

bool String::EndsWithChar(const uint8_t byte) const noexcept {
  if (len_ == 0) [[unlikely]] {
    return false;
  }

  return buf_[len_ - 1] == byte;
}

bool String::EndsWithAnyOfChars(const char *charset) const noexcept {
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

void String::AppendChar(uint8_t c) noexcept {
  if (len_ + 1 == cap_) [[unlikely]] {
    Grow(cap_ * 2);
  }

  buf_[len_] = c;
  buf_[len_ + 1] = 0;
  ++len_;
}

// before appending:
// *buf_ = 0000000000000000 (cap_ = 16, len_ = 0)
//         ^
//         len_
//
// example: appending "hello world" (strlen = 11)
//  after memcpy: *buf_ = [hello world00000], len_ = 11
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

void String::AppendStringView(std::string_view sv) noexcept {
  const auto l = sv.size();

  if (len_ + l >= cap_) [[unlikely]] {
    Grow(RoundUp8(len_ + l + 1));
  }

  memcpy(reinterpret_cast<uint8_t *>(buf_ + len_), reinterpret_cast<const uint8_t *>(sv.data()), l);
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
//  MemZeroPtrRange(buf_ + len_, buf_ + new_cap);
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

bool String::compare(const char *cstr, const uint64_t length,
                     const uint64_t offset) const noexcept {
  if (offset + length > len_) {
    return false;
  }

  if (length == 0) [[unlikely]] {
    return true;
  }

  return MemCompare(cstr, buf_ + offset, length);
}

bool String::compareIgnoreCase(const char *cstr, const uint64_t length,
                               const uint64_t offset) const noexcept {
  if (offset + length > len_) {
    return false;
  }

  if (length == 0) [[unlikely]] {
    return true;
  }

  for (uint64_t i = 0; i < length; ++i) {
    const uint8_t a{cell::ToLower(buf_[i])};
    const uint8_t b{cell::ToLower(buf_[i])};

    if (a != b) {
      return false;
    }
  }

  return true;
}

bool String::isAnyOf(const uint8_t candidate, const char *charset) noexcept {
  while (*charset) {
    if (*charset == candidate) {
      return true;
    }

    ++charset;
  }

  return false;
}

}  // namespace cell