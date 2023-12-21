// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_HTTPURI_HPP
#define CELL_HTTPURI_HPP

#include <cstdint>
#include <functional>
#include <unordered_map>

#include "cell/String.hpp"
#include "cell/WeakStringMap.hpp"

namespace cell::http {

class HttpUri {
 public:
  explicit HttpUri() noexcept = default;

  void SetBufferContents(const String& other) noexcept {
    path_ = other;
  }

  StringSlice GetPath() const noexcept {
    return path_.SubSlice();
  }

 private:
  static constexpr uint64_t kDefaultUriBufferCapacity = 4096;

  String path_{kDefaultUriBufferCapacity};
  WeakStringMap queries_;
};

}  // namespace cell::http

#endif  // CELL_HTTPURI_HPP
