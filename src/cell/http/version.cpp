// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include "version.hpp"

#include "cell/core/assert.hpp"

namespace cell::http {

Version version_from_string(StringSlice slice) noexcept {
  if (slice.Compare(StringSlice::from_cstr("HTTP/1"))) {
    return Version::Http1;
  } else if (slice.Compare(StringSlice::from_cstr("HTTP/1.1"))) {
    return Version::Http1_1;
  } else if (slice.Compare(StringSlice::from_cstr("HTTP/2"))) {
    return Version::Http2;
  } else if (slice.Compare(StringSlice::from_cstr("HTTP/3"))) {
    return Version::Http3;
  } else {
    return Version::UnsupportedVersion;
  }
}

StringSlice version_to_string(Version version) noexcept {
  switch (version) {
    case Version::Http1:
      return StringSlice::from_cstr("HTTP/1");
    case Version::Http1_1:
      return StringSlice::from_cstr("HTTP/1.1");
    case Version::Http2:
      return StringSlice::from_cstr("HTTP/2");
    case Version::Http3:
      return StringSlice::from_cstr("HTTP/3");
    case Version::UnsupportedVersion:
      CELL_PANIC("invalid http version");
  }
}
}  // namespace cell::http