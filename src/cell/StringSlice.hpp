// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_STRINGSLICE_HPP
#define CELL_STRINGSLICE_HPP

#include <cstdint>

namespace cell {

class StringSlice {
 public:
  explicit StringSlice(const uint8_t* data);
  StringSlice(const uint8_t* data, uint64_t len);

  StringSlice(const StringSlice& other) = default;
  StringSlice(StringSlice&& other) = default;
  StringSlice& operator=(const StringSlice& other) = default;
  StringSlice& operator=(StringSlice&& other) = default;

  [[nodiscard]] static StringSlice FromCString(const char* data) noexcept;
  [[nodiscard]] static StringSlice FromCString(const char* data, uint64_t len) noexcept;

  [[nodiscard]] const char* GetConstCharPtr() const { return reinterpret_cast<const char*>(data_); }
  [[nodiscard]] const uint8_t* GetU8Ptr() const { return data_; }
  [[nodiscard]] uint64_t GetLen() const { return len_; }

  [[nodiscard]] bool Compare(StringSlice to) const noexcept;
  [[nodiscard]] bool CompareIgnoreCase(StringSlice to) const noexcept;
  [[nodiscard]] bool Contains(uint8_t byte) const noexcept;

 private:
  const uint8_t* data_{nullptr};
  uint64_t len_{0};
};

}  // namespace cell

#endif  // CELL_STRINGSLICE_HPP