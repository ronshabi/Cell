// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include "cell/Charset.hpp"
#include "cell/Memory.hpp"
#include "cell/String.hpp"

using cell::String;

TEST(StringTest, Basic1) {
  String myString;
  myString.AppendCString("GET / HTTP/1.1");

  ASSERT_EQ(myString.GetLen(), 14);
  ASSERT_EQ(myString.GetCap(), 16);
  ASSERT_STREQ(myString.GetCString(), "GET / HTTP/1.1");
  ASSERT_TRUE(myString.Compare("GET / HTTP/1.1"));
  ASSERT_TRUE(myString.CompareIgnoreCase("Get / http/1.1"));
  ASSERT_TRUE(myString.Contains('/'));
  ASSERT_FALSE(myString.Contains('@'));
  ASSERT_FALSE(myString.Contains("123456789"));
  ASSERT_TRUE(myString.ContainsIgnoreCase("http"));
  ASSERT_TRUE(myString.ContainsAnyOf("/. 1"));
  ASSERT_TRUE(myString.ContainsAnyOf("123456789"));
  ASSERT_TRUE(myString.ContainsAnyOf(cell::kAsciiUpper));
  ASSERT_FALSE(myString.ContainsJust(cell::kAsciiUpper));
  ASSERT_TRUE(myString.StartsWith("GET"));
  ASSERT_TRUE(myString.StartsWithIgnoreCase("gEt"));
  ASSERT_TRUE(myString.StartsWithChar('G'));
  ASSERT_FALSE(myString.StartsWithAnyOfChars(cell::kAsciiWhitespace));
  ASSERT_TRUE(myString.EndsWithChar('1'));
  ASSERT_TRUE(myString.EndsWithAnyOfChars(cell::kDigits));

  myString.ToLower();
  myString.ReplaceAnyOf({" / ", "/1.1"}, "");
  //  std::printf("The string is: [%s]\n", myString.GetCString());
  ASSERT_TRUE(myString.ContainsJust(cell::kAsciiLower));
  myString.AppendInt64(69420);
  ASSERT_TRUE(myString.CompareIgnoreCase("gethTTp69420"));
  myString.AppendCString("    ");
  ASSERT_TRUE(myString.Compare("gethttp69420    "));
  ASSERT_EQ(myString.GetLen(), 16);

  myString.TrimRight();
  std::printf("The string is: [%s]\n", myString.GetCString());
  ASSERT_TRUE(myString.Compare("gethttp69420"));
  ASSERT_EQ(myString.GetLen(), 12);

  const auto current_cap = myString.GetCap();

  myString.Clear();
  ASSERT_EQ(myString.GetLen(), 0);
  ASSERT_EQ(myString.GetCap(), current_cap);
  ASSERT_TRUE(myString.ContainsJust(0));
}