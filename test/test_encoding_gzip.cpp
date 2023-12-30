// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include "cell/core/string.hpp"
#include "cell/core/string_slice.hpp"
#include "cell/encoding/gzip.hpp"

using cell::String;
using cell::StringSlice;

TEST(encoding_gzip, simple) {
  String src(StringSlice::from_cstr("HTTP/1.1 200 OK"));
  String dest(16);

  cell::encoding::Gzip gzip;
  ASSERT_TRUE(gzip.is_ok());
}