// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include "HttpVersion.hpp"

#include "cell/Assert.hpp"

namespace cell::http {

HttpVersion HttpVersionFromString(StringSlice slice) noexcept {
  if (slice.Compare(StringSlice::FromCString("HTTP/1"))) {
    return HttpVersion::Http1;
  } else if (slice.Compare(StringSlice::FromCString("HTTP/1.1"))) {
    return HttpVersion::Http1_1;
  } else if (slice.Compare(StringSlice::FromCString("HTTP/2"))) {
    return HttpVersion::Http2;
  } else if (slice.Compare(StringSlice::FromCString("HTTP/3"))) {
    return HttpVersion::Http3;
  } else {
    return HttpVersion::UnsupportedVersion;
  }
}

StringSlice HttpVersionToString(HttpVersion version) noexcept {
  switch (version) {
    case HttpVersion::Http1:
      return StringSlice::FromCString("HTTP/1");
    case HttpVersion::Http1_1:
      return StringSlice::FromCString("HTTP/1.1");
    case HttpVersion::Http2:
      return StringSlice::FromCString("HTTP/2");
    case HttpVersion::Http3:
      return StringSlice::FromCString("HTTP/3");
    case HttpVersion::UnsupportedVersion:
      CELL_PANIC("invalid http version");
  }
}
}  // namespace cell::http