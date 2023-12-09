// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_HTTPMETHOD_HPP
#define CELL_HTTPMETHOD_HPP

#include "cell/StringSlice.hpp"

namespace cell::http {

enum class HttpMethod { Get, Post, Head, UnsupportedMethod };

HttpMethod HttpMethodFromString(StringSlice s) noexcept;
StringSlice HttpMethodToString(HttpMethod method) noexcept;
}  // namespace cell::http

#endif  // CELL_HTTPMETHOD_HPP
