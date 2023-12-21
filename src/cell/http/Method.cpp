// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include "Method.hpp"

#include "cell/core/Assert.hpp"

namespace cell::http {

Method HttpMethodFromString(const StringSlice s) noexcept {
  if (s.Compare(StringSlice::FromCString("GET"))) {
    return Method::Get;
  } else if (s.Compare(StringSlice::FromCString("POST"))) {
    return Method::Post;
  } else if (s.Compare(StringSlice::FromCString("HEAD"))) {
    return Method::Head;
  } else {
    return Method::UnsupportedMethod;
  }
}

StringSlice HttpMethodToString(const Method method) noexcept {
  switch (method) {
    case Method::Get:
      return StringSlice::FromCString("GET");
    case Method::Post:
      return StringSlice::FromCString("POST");
    case Method::Head:
      return StringSlice::FromCString("HEAD");
    case Method::UnsupportedMethod:
      CELL_PANIC("unknown method");
  }
}

}  // namespace cell::http