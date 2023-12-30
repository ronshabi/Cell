// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_VERSION_HPP
#define CELL_VERSION_HPP

#include <cstdint>

#include "cell/core/string_slice.hpp"

namespace cell::http {

enum class Version { Http1, Http1_1, Http2, Http3, UnsupportedVersion };

[[nodiscard]] Version version_from_string(StringSlice slice) noexcept;
[[nodiscard]] StringSlice version_to_string(Version version) noexcept;

}  // namespace cell::http

#endif  // CELL_VERSION_HPP
