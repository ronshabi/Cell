// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_HTTPVERSION_HPP
#define CELL_HTTPVERSION_HPP

#include <cstdint>

#include "cell/StringSlice.hpp"

namespace cell::http {

enum class HttpVersion { Http1, Http1_1, Http2, Http3, UnsupportedVersion };

[[nodiscard]] HttpVersion HttpVersionFromString(StringSlice slice) noexcept;
[[nodiscard]] StringSlice HttpVersionToString(HttpVersion version) noexcept;

}  // namespace cell::http

#endif  // CELL_HTTPVERSION_HPP
