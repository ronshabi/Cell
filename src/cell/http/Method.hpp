// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_METHOD_HPP
#define CELL_METHOD_HPP

#include "cell/StringSlice.hpp"

namespace cell::http {

enum class Method { Get, Post, Head, UnsupportedMethod };

Method HttpMethodFromString(StringSlice s) noexcept;
StringSlice HttpMethodToString(Method method) noexcept;
}  // namespace cell::http

#endif  // CELL_METHOD_HPP
