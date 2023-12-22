// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_STRING_HPP
#define CELL_STRING_HPP

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "Assert.hpp"
#include "StringSlice.hpp"

namespace cell {

class String {
 public:
  friend class Scanner;
  friend class StringSlice;

  explicit String() noexcept;
  explicit String(uint64_t initial_capacity_hint) noexcept;
  explicit String(StringSlice slice) noexcept;
  String(const String &other) noexcept;
  String(String &&other) noexcept;
  String &operator=(const String &other) noexcept;
  String &operator=(String &&other) noexcept;
  ~String();

  [[nodiscard]] constexpr uint8_t *GetBufPtr() noexcept { return buf_; }
  [[nodiscard]] constexpr uint8_t *GetBufPtr() const noexcept { return buf_; }
  [[nodiscard]] constexpr uint64_t GetCap() const noexcept { return cap_; }
  [[nodiscard]] constexpr uint64_t GetLen() const noexcept { return len_; }
  [[nodiscard]] constexpr uint8_t ByteAt(uint64_t i) const noexcept {
    CELL_ASSERT(i < len_);
    return buf_[i];
  }

  [[nodiscard]] constexpr bool IsEmpty() const noexcept {
    return len_ == 0;
  }

  [[nodiscard]] bool Compare(StringSlice str) const noexcept;
  [[nodiscard]] bool CompareIgnoreCase(StringSlice slice) const noexcept;
  [[nodiscard]] bool Contains(uint8_t byte) const noexcept;
  [[nodiscard]] bool Contains(StringSlice slice) const noexcept;
  [[nodiscard]] bool ContainsIgnoreCase(StringSlice slice) const noexcept;
  [[nodiscard]] bool ContainsAnyOf(StringSlice slice) const noexcept;
  [[nodiscard]] bool ContainsAnyOf(const std::unordered_set<uint8_t> &charset) const noexcept;
  [[nodiscard]] bool ContainsJust(StringSlice charset) const noexcept;
  [[nodiscard]] bool ContainsJust(const std::unordered_set<uint8_t> &charset) const noexcept;

  void ReplaceChar(uint8_t original, uint8_t replacement) noexcept;
  void ReplaceAnyOfChars(StringSlice charset, uint8_t replacement) noexcept;
  void Replace(StringSlice candidate, StringSlice replacement) noexcept;
  void ReplaceAnyOf(const std::vector<StringSlice> &candidates, StringSlice replacement) noexcept;

  [[nodiscard]] bool StartsWithChar(uint8_t byte) const noexcept;
  [[nodiscard]] bool StartsWithAnyOfChars(StringSlice charset) const noexcept;
  [[nodiscard]] bool StartsWith(StringSlice slice) const noexcept;
  [[nodiscard]] bool StartsWithIgnoreCase(StringSlice slice) const noexcept;
  [[nodiscard]] bool EndsWithChar(uint8_t byte) const noexcept;
  [[nodiscard]] bool EndsWithAnyOfChars(StringSlice charset) const noexcept;

  void ToLower() noexcept;
  void ToUpper() noexcept;
  void Trim(uint8_t delimiter = ' ') noexcept;
  void TrimLeft(uint8_t delimiter = ' ') noexcept;
  void TrimRight(uint8_t delimiter = ' ') noexcept;
  void Clear() noexcept;

  void AppendByte(uint8_t c) noexcept;
  void AppendCString(const char *cstr) noexcept;
  void AppendStringSlice(StringSlice slice) noexcept;
  void AppendString(const String &other) noexcept;
  void AppendInt32(const int32_t num) noexcept { AppendSprintf<32>("%ld", num); }
  void AppendInt64(const int64_t num) noexcept { AppendSprintf<32>("%lld", num); }
  void AppendUInt32(const uint32_t num) noexcept { AppendSprintf<32>("%lu", num); }
  void AppendUInt64(const uint64_t num) noexcept { AppendSprintf<32>("%llu", num); }

  // The printf buffer you will append to is fixed, and is determined by
  // the SprintfBufferSize template argument. Use with caution.
  template <uint64_t SprintfBufferSize, typename... Args>
  void AppendSprintf(const char *fmt, Args &&...args) noexcept {
    char tmp[SprintfBufferSize];
    memset(tmp, 0, SprintfBufferSize);
    std::sprintf(tmp, fmt, std::forward<Args>(args)...);
    AppendCString(tmp);
  }

  [[nodiscard]] bool AppendFileContents(const char *path) noexcept;
  [[nodiscard]] bool SaveToFile(const char *path) const noexcept;

  void Grow(uint64_t new_cap);
  void RefreshLength() noexcept;

  // Returns a const pointer to the buffer, which is guaranteed to be a C
  // compatible string (null terminated string)
  [[nodiscard]] const char *GetCString() const noexcept {
    return reinterpret_cast<const char *>(buf_);
  }

  // Returns a const ptr to the c string, skipping first N delimiters.
  // Note: this function just returns a POINTER to the buffer, and does NOT
  // actually change it. Use with caution.
  [[nodiscard]] const char *GetCStringTrimmedLeft(uint8_t delimiter = ' ') const noexcept;

  [[nodiscard]] StringSlice SubSlice() const noexcept;
  [[nodiscard]] StringSlice SubSlice(uint64_t from, uint64_t n) const noexcept;
  [[nodiscard]] StringSlice SubSlice(uint64_t from) const noexcept;

  // Creates new std::string from buffer
  [[nodiscard]] std::string GetStdString() const noexcept { return GetCString(); }

 private:
  [[nodiscard]] bool compare(const uint8_t *data, uint64_t length, uint64_t offset) const noexcept;
  [[nodiscard]] bool compareIgnoreCase(const uint8_t *data, uint64_t length,
                                       uint64_t offset) const noexcept;
  [[nodiscard]] static bool isAnyOf(uint8_t candidate, StringSlice charset) noexcept;

  uint64_t cap_{};
  uint64_t len_{};
  uint8_t *buf_{};
};

}  // namespace cell

#endif  // CELL_STRING_HPP
