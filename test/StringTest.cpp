// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <cstdio>
#include "cell/Charset.hpp"
#include "cell/Memory.hpp"
#include "cell/String.hpp"
#include "cell/StringSlice.hpp"

using cell::String;
using cell::StringSlice;

TEST(StringTest, Basic1) {
  String myString;
  myString.AppendCString("GET / HTTP/1.1");

  ASSERT_EQ(myString.GetLen(), 14);
  ASSERT_EQ(myString.GetCap(), 16);
  ASSERT_STREQ(myString.GetCString(), "GET / HTTP/1.1");
  ASSERT_TRUE(myString.Compare(StringSlice::FromCString("GET / HTTP/1.1")));
  ASSERT_TRUE(myString.CompareIgnoreCase(StringSlice::FromCString("Get / http/1.1")));
  ASSERT_TRUE(myString.Contains('/'));
  ASSERT_FALSE(myString.Contains('@'));
  ASSERT_FALSE(myString.Contains(StringSlice::FromCString("123456789")));
  ASSERT_TRUE(myString.ContainsIgnoreCase(StringSlice::FromCString("http")));
  ASSERT_FALSE(myString.ContainsAnyOf(StringSlice::FromCString("http")));
  ASSERT_TRUE(myString.ContainsAnyOf(StringSlice::FromCString("123456789")));
  ASSERT_TRUE(myString.ContainsAnyOf(StringSlice::FromCString(cell::kAsciiUpper)));
  ASSERT_FALSE(myString.ContainsJust(StringSlice::FromCString(cell::kAsciiUpper)));
  ASSERT_TRUE(myString.StartsWith(StringSlice::FromCString("GET")));
  ASSERT_TRUE(myString.StartsWithIgnoreCase(StringSlice::FromCString("gEt")));
  ASSERT_TRUE(myString.StartsWithChar('G'));
  ASSERT_FALSE(myString.StartsWithAnyOfChars(StringSlice::FromCString(cell::kAsciiWhitespace)));
  ASSERT_TRUE(myString.EndsWithChar('1'));
  ASSERT_TRUE(myString.EndsWithAnyOfChars(StringSlice::FromCString(cell::kDigits)));

  myString.ToLower();
  myString.ReplaceAnyOf({StringSlice::FromCString(" / "), StringSlice::FromCString("/1.1")}, StringSlice::FromCString(""));
  //  std::printf("The string is: [%s]\n", myString.GetCString());
  ASSERT_TRUE(myString.ContainsJust(StringSlice::FromCString(cell::kAsciiLower)));
  myString.AppendInt64(69420);
  ASSERT_TRUE(myString.CompareIgnoreCase(StringSlice::FromCString("gethTTp69420")));
  myString.AppendCString("    ");
  ASSERT_TRUE(myString.Compare(StringSlice::FromCString("gethttp69420    ")));
  ASSERT_EQ(myString.GetLen(), 16);

  myString.TrimRight();
//  std::printf("The string is: [%s]\n", myString.GetCString());
  ASSERT_TRUE(myString.Compare(StringSlice::FromCString("gethttp69420")));
  ASSERT_EQ(myString.GetLen(), 12);

  const auto current_cap = myString.GetCap();

  myString.Clear();
  ASSERT_EQ(myString.GetLen(), 0);
  ASSERT_EQ(myString.GetCap(), current_cap);
  ASSERT_TRUE(myString.ContainsJust({0}));
}

TEST(StringTest, EmptyFile) {
  String myString;
  ASSERT_TRUE(myString.AppendFileContents("StringCorpus/Empty.input"));
  ASSERT_EQ(myString.GetLen(), 0);
  myString.ReplaceAnyOf({StringSlice::FromCString(" "), StringSlice::FromCString("\n")}, StringSlice::FromCString(""));
  ASSERT_EQ(myString.GetLen(), 0);
  ASSERT_EQ(myString.GetCap(), 8);
}

TEST(StringTest, AlotOfHebrew) {
  String myString;
  ASSERT_TRUE(myString.AppendFileContents("StringCorpus/AlotOfHebrew.input"));
  ASSERT_EQ(myString.GetLen(), 12306);

  myString.ReplaceAnyOf({StringSlice::FromCString(" "), StringSlice::FromCString("\n")}, StringSlice::FromCString(""));
//  fprintf(stderr, "[%s]\n", myString.GetCString());
  ASSERT_EQ(myString.GetLen(), 6102);
}

TEST(StringTest, NonAsciiChars) {
  String myString;
  ASSERT_TRUE(myString.AppendFileContents("StringCorpus/NonAsciiChars.input"));
  ASSERT_EQ(myString.GetLen(), 146);
  myString.ReplaceAnyOf({StringSlice::FromCString(" ")}, StringSlice::FromCString(""));
  ASSERT_EQ(myString.GetLen(), 125);
}


