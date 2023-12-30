// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <cell/core/string.hpp>
#include <cell/core/string_slice.hpp>
#include <cell/http/uri.hpp>

using cell::String;
using cell::StringSlice;
using cell::http::Uri;

constexpr auto kOutDefaultCapacity = 32;

TEST(UriDecodeTest, ShouldNotDecodeOnlyAscii) {
  String out(kOutDefaultCapacity);
  const auto encoded_uri =
      StringSlice::from_cstr("main/articles/view_page/article.php?article=32~");

  ASSERT_TRUE(Uri::decode(encoded_uri, out));
  ASSERT_STREQ(out.get_c_str(), encoded_uri.GetConstCharPtr());
}

TEST(UriDecodeTest, SimpleAsciiDecoding) {
  String out(kOutDefaultCapacity);
  const auto encoded_uri =
      StringSlice::from_cstr("main/articles/view_page/article.php?%25article=32~");

  ASSERT_TRUE(Uri::decode(encoded_uri, out));
  ASSERT_STREQ(out.get_c_str(), "main/articles/view_page/article.php?%article=32~");
}

TEST(UriDecodeTest, JustOneCharacter) {
  String out(kOutDefaultCapacity);
  const auto encoded_uri = StringSlice::from_cstr("%2F");

  ASSERT_TRUE(Uri::decode(encoded_uri, out));
  ASSERT_STREQ(out.get_c_str(), "/");
}

TEST(UriDecodeTest, JustOneCharacterAtTheEnd) {
  String out(kOutDefaultCapacity);
  const auto encoded_uri = StringSlice::from_cstr("/index.asp?%41");

  ASSERT_TRUE(Uri::decode(encoded_uri, out));
  ASSERT_STREQ(out.get_c_str(), "/index.asp?A");
}

TEST(UriDecodeTest, ShouldFailPercentWithoutHexdigs) {
  String out(kOutDefaultCapacity);
  const auto encoded_uri = StringSlice::from_cstr("/index.asp?%OO/blah/123");

  ASSERT_FALSE(Uri::decode(encoded_uri, out));
}

TEST(UriDecodeTest, ShouldFailPercentAtTheEnd) {
  String out(kOutDefaultCapacity);
  const auto encoded_uri = StringSlice::from_cstr("/index.asp?%3");

  ASSERT_FALSE(Uri::decode(encoded_uri, out));
}

TEST(UriDecodeTest, SimpleUtf8) {
  String out(kOutDefaultCapacity);
  const auto encoded_uri = StringSlice::from_cstr("/%D7%A9%D7%9C%D7%95%D7%9D");

  ASSERT_TRUE(Uri::decode(encoded_uri, out));
  ASSERT_STREQ(out.get_c_str(), "/שלום");
}