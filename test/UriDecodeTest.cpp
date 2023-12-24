// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <cell/http/Uri.hpp>
#include <cell/core/String.hpp>
#include <cell/core/StringSlice.hpp>

using cell::String;
using cell::StringSlice;
using cell::http::Uri;

constexpr auto kOutDefaultCapacity = 32;

TEST(UriDecodeTest, ShouldNotDecodeOnlyAscii) {
  String out(kOutDefaultCapacity);
  const auto encoded_uri = StringSlice::FromCString("main/articles/view_page/article.php?article=32~");

  ASSERT_TRUE(Uri::Decode(encoded_uri, out));
  ASSERT_STREQ(out.GetCString(), encoded_uri.GetConstCharPtr());
}

TEST(UriDecodeTest, SimpleAsciiDecoding) {
  String out(kOutDefaultCapacity);
  const auto encoded_uri = StringSlice::FromCString("main/articles/view_page/article.php?%25article=32~");

  ASSERT_TRUE(Uri::Decode(encoded_uri, out));
  ASSERT_STREQ(out.GetCString(), "main/articles/view_page/article.php?%article=32~");
}

TEST(UriDecodeTest, JustOneCharacter) {
  String out(kOutDefaultCapacity);
  const auto encoded_uri = StringSlice::FromCString("%2F");

  ASSERT_TRUE(Uri::Decode(encoded_uri, out));
  ASSERT_STREQ(out.GetCString(), "/");
}

TEST(UriDecodeTest, JustOneCharacterAtTheEnd) {
  String out(kOutDefaultCapacity);
  const auto encoded_uri = StringSlice::FromCString("/index.asp?%41");

  ASSERT_TRUE(Uri::Decode(encoded_uri, out));
  ASSERT_STREQ(out.GetCString(), "/index.asp?A");
}

TEST(UriDecodeTest, ShouldFailPercentWithoutHexdigs) {
  String out(kOutDefaultCapacity);
  const auto encoded_uri = StringSlice::FromCString("/index.asp?%OO/blah/123");

  ASSERT_FALSE(Uri::Decode(encoded_uri, out));
}

TEST(UriDecodeTest, ShouldFailPercentAtTheEnd) {
  String out(kOutDefaultCapacity);
  const auto encoded_uri = StringSlice::FromCString("/index.asp?%3");

  ASSERT_FALSE(Uri::Decode(encoded_uri, out));
}

TEST(UriDecodeTest, SimpleUtf8) {
  String out(kOutDefaultCapacity);
  const auto encoded_uri = StringSlice::FromCString("/%D7%A9%D7%9C%D7%95%D7%9D");

  ASSERT_TRUE(Uri::Decode(encoded_uri, out));
  ASSERT_STREQ(out.GetCString(), "/שלום");
}