// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_STRBUF_HPP
#define CELL_STRBUF_HPP

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace cell {

class Strbuf {
 public:
  explicit Strbuf(uint64_t cap) noexcept;
  Strbuf(const Strbuf &other) noexcept;
  Strbuf(Strbuf &&other) noexcept;
  Strbuf &operator=(const Strbuf &other) noexcept;
  Strbuf &operator=(Strbuf &&other) noexcept;
  ~Strbuf();

  [[nodiscard]] constexpr uint8_t *GetBufPtr() noexcept { return buf_; }
  [[nodiscard]] constexpr uint8_t *GetBufPtr() const noexcept { return buf_; }
  [[nodiscard]] constexpr uint64_t GetCap() const noexcept { return cap_; }
  [[nodiscard]] constexpr uint64_t GetLen() const noexcept { return len_; }

  [[nodiscard]] bool Compare(const char *cstr) const noexcept;
  [[nodiscard]] bool Compare(std::string_view str) const noexcept;
  [[nodiscard]] bool CompareIgnoreCase(const char *cstr) const noexcept;
  [[nodiscard]] bool CompareIgnoreCase(std::string_view sv) const noexcept;
  [[nodiscard]] bool Contains(uint8_t byte) const noexcept;
  [[nodiscard]] bool Contains(const char *cstr) const noexcept;
  [[nodiscard]] bool Contains(std::string_view sv) const noexcept;
  [[nodiscard]] bool ContainsIgnoreCase(const char *cstr) const noexcept;
  [[nodiscard]] bool ContainsIgnoreCase(std::string_view sv) const noexcept;
  [[nodiscard]] bool ContainsAnyOf(const char *charset) const noexcept;
  [[nodiscard]] bool ContainsAnyOf(const std::unordered_set<uint8_t> &charset) const noexcept;
  [[nodiscard]] bool ContainsJust(const char *charset) const noexcept;
  [[nodiscard]] bool ContainsJust(const std::unordered_set<uint8_t> &charset) const noexcept;

  void ReplaceChar(uint8_t original, uint8_t replacement) noexcept;
  void ReplaceAnyOfChars(const char *charset, uint8_t replacement) noexcept;
  void Replace(const char *candidate, const char *replacement) noexcept;
  void ReplaceAnyOf(const std::vector<const char *> &candidates, const char *replacement) noexcept;
  void ReplaceHashmap(const std::unordered_map<const char *, const char *> &hashmap) noexcept;

  [[nodiscard]] bool StartsWithChar(uint8_t byte) const noexcept;
  [[nodiscard]] bool StartsWithAnyOfChars(const char *charset) const noexcept;
  [[nodiscard]] bool StartsWith(const char *cstr) const noexcept;
  [[nodiscard]] bool StartsWithIgnoreCase(const char *cstr) const noexcept;
  [[nodiscard]] bool EndsWithChar(uint8_t byte) const noexcept;
  [[nodiscard]] bool EndsWithAnyOfChars(const char *charset) const noexcept;

  // TODO
  //  [[nodiscard]] bool IsLower() const noexcept;
  //  [[nodiscard]] bool IsUpper() const noexcept;

  void ToLower() noexcept;
  void ToUpper() noexcept;
  void Trim(uint8_t delimiter = ' ') noexcept;
  void TrimLeft(uint8_t delimiter = ' ') noexcept;
  void TrimRight(uint8_t delimiter = ' ') noexcept;
  void Clear() noexcept;

  void AppendChar(uint8_t c) noexcept;
  void AppendCString(const char *cstr) noexcept;
  void AppendStringView(std::string_view sv) noexcept;
  void AppendStrbuf(const Strbuf &other) noexcept;
  void AppendInt32(const int32_t num) noexcept { AppendSprintf<32>("%ld", num); }
  void AppendInt64(const int64_t num) noexcept { AppendSprintf<32>("%lld", num); }
  void AppendUInt32(const uint32_t num) noexcept { AppendSprintf<32>("%lu", num); }
  void AppendUInt64(const uint64_t num) noexcept { AppendSprintf<32>("%llu", num); }

  [[nodiscard]] bool AppendFileContents(const char *path) noexcept;
  [[nodiscard]] bool SaveToFile(const char *path) const noexcept;

  // The printf buffer you will append to is fixed, and is determined by
  // the PrintfBufferSize template argument. Use with caution.
  template <uint64_t SprintfBufferSize, typename... Args>
  void AppendSprintf(const char *fmt, Args &&...args) noexcept {
    char tmp[SprintfBufferSize];
    memset(tmp, 0, SprintfBufferSize);
    std::sprintf(tmp, fmt, std::forward<Args>(args)...);
    AppendCString(tmp);
  }

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

  // Creates new std::string from buffer
  [[nodiscard]] std::string GetStdString() const noexcept { return GetCString(); }

 private:
  [[nodiscard]] bool compare(const char *cstr, uint64_t length, uint64_t offset) const noexcept;
  [[nodiscard]] bool compareIgnoreCase(const char *cstr, uint64_t length,
                                       uint64_t offset) const noexcept;
  [[nodiscard]] static bool isAnyOf(uint8_t candidate, const char *charset) noexcept;

  uint64_t cap_{};
  uint64_t len_{};
  uint8_t *buf_{};
};

}  // namespace cell

#endif  // CELL_STRBUF_HPP
