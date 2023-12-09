// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include "HttpMethod.hpp"

#include "cell/Assert.hpp"

namespace cell::http {

HttpMethod HttpMethodFromString(const StringSlice s) noexcept {
  if (s.Compare(StringSlice::FromCString("GET"))) {
    return HttpMethod::Get;
  } else if (s.Compare(StringSlice::FromCString("POST"))) {
    return HttpMethod::Post;
  } else if (s.Compare(StringSlice::FromCString("HEAD"))) {
    return HttpMethod::Head;
  } else {
    return HttpMethod::UnsupportedMethod;
  }
}

StringSlice HttpMethodToString(const HttpMethod method) noexcept {
  switch (method) {
    case HttpMethod::Get:
      return StringSlice::FromCString("GET");
    case HttpMethod::Post:
      return StringSlice::FromCString("POST");
    case HttpMethod::Head:
      return StringSlice::FromCString("HEAD");
    case HttpMethod::UnsupportedMethod:
      CELL_PANIC("unknown method");
  }
}

}  // namespace cell::http