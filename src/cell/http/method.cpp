// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include "method.hpp"

#include "cell/core/assert.hpp"

namespace cell::http {

Method method_from_string(StringSlice s) noexcept {
  if (s.Compare(StringSlice::from_cstr("GET"))) {
    return Method::Get;
  } else if (s.Compare(StringSlice::from_cstr("POST"))) {
    return Method::Post;
  } else if (s.Compare(StringSlice::from_cstr("HEAD"))) {
    return Method::Head;
  } else {
    return Method::UnsupportedMethod;
  }
}

StringSlice method_to_string(Method method) noexcept {
  switch (method) {
    case Method::Get:
      return StringSlice::from_cstr("GET");
    case Method::Post:
      return StringSlice::from_cstr("POST");
    case Method::Head:
      return StringSlice::from_cstr("HEAD");
    case Method::UnsupportedMethod:
      CELL_PANIC("unknown method");
  }
}

}  // namespace cell::http