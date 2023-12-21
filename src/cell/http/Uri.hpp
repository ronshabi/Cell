// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_URI_HPP
#define CELL_URI_HPP

#include <cstdint>
#include <functional>
#include <unordered_map>

#include "cell/String.hpp"
#include "cell/WeakStringCache.hpp"

namespace cell::http {

class Uri {
 public:
  explicit Uri() noexcept = default;

  void SetBufferContents(const String& other) noexcept {
    path_ = other;
  }

  StringSlice GetPath() const noexcept {
    return path_.SubSlice();
  }

 private:
  static constexpr uint64_t kDefaultUriBufferCapacity = 4096;

  String path_{kDefaultUriBufferCapacity};
  WeakStringCache queries_;
};

}  // namespace cell::http

#endif  // CELL_URI_HPP
