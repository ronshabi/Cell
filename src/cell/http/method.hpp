// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_METHOD_HPP
#define CELL_METHOD_HPP

#include "cell/core/string_slice.hpp"

namespace cell::http {

enum class Method { Get, Post, Head, UnsupportedMethod };

Method method_from_string(StringSlice s) noexcept;
StringSlice method_to_string(Method method) noexcept;
}  // namespace cell::http

#endif  // CELL_METHOD_HPP
