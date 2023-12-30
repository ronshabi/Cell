// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <cstdio>

#include "cell/core/charset.hpp"
#include "cell/core/memory.hpp"
#include "cell/core/string.hpp"
#include "cell/core/string_slice.hpp"

using cell::String;
using cell::StringSlice;

TEST(StringTest, Basic1) {
  String myString;
  myString.append_c_str("GET / HTTP/1.1");

  ASSERT_EQ(myString.get_length(), 14);
  ASSERT_EQ(myString.get_capacity(), 16);
  ASSERT_STREQ(myString.get_c_str(), "GET / HTTP/1.1");
  ASSERT_TRUE(myString.compare(StringSlice::from_cstr("GET / HTTP/1.1")));
  ASSERT_TRUE(myString.compare_ignore_case(StringSlice::from_cstr("Get / http/1.1")));
  ASSERT_TRUE(myString.contains('/'));
  ASSERT_FALSE(myString.contains('@'));
  ASSERT_FALSE(myString.contains(StringSlice::from_cstr("123456789")));
  ASSERT_TRUE(myString.contains_ignore_case(StringSlice::from_cstr("http")));
  ASSERT_FALSE(myString.contains_any_of(StringSlice::from_cstr("http")));
  ASSERT_TRUE(myString.contains_any_of(StringSlice::from_cstr("123456789")));
  ASSERT_TRUE(myString.contains_any_of(StringSlice::from_cstr(cell::ASCII_UPPER)));
  ASSERT_FALSE(myString.contains_just(StringSlice::from_cstr(cell::ASCII_UPPER)));
  ASSERT_TRUE(myString.starts_with(StringSlice::from_cstr("GET")));
  ASSERT_TRUE(myString.starts_with_ignore_case(StringSlice::from_cstr("gEt")));
  ASSERT_TRUE(myString.starts_with_byte('G'));
  ASSERT_FALSE(myString.starts_with_any_of_byte(StringSlice::from_cstr(cell::ASCII_WHITESPACE)));
  ASSERT_TRUE(myString.ends_with_byte('1'));
  ASSERT_TRUE(myString.ends_with_any_of_bytes(StringSlice::from_cstr(cell::DIGITS)));

  myString.to_lower();
  myString.replace_any_of({StringSlice::from_cstr(" / "), StringSlice::from_cstr("/1.1")},
                          StringSlice::from_cstr(""));
  //  std::printf("The string is: [%s]\n", myString.get_c_str());
  ASSERT_TRUE(myString.contains_just(StringSlice::from_cstr(cell::kAsciiLower)));
  myString.append_i64(69420);
  ASSERT_TRUE(myString.compare_ignore_case(StringSlice::from_cstr("gethTTp69420")));
  myString.append_c_str("    ");
  ASSERT_TRUE(myString.compare(StringSlice::from_cstr("gethttp69420    ")));
  ASSERT_EQ(myString.get_length(), 16);

  myString.trim_right();
//  std::printf("The string is: [%s]\n", myString.get_c_str());
  ASSERT_TRUE(myString.compare(StringSlice::from_cstr("gethttp69420")));
  ASSERT_EQ(myString.get_length(), 12);

  const auto current_cap = myString.get_capacity();

  myString.clear();
  ASSERT_EQ(myString.get_length(), 0);
  ASSERT_EQ(myString.get_capacity(), current_cap);
  ASSERT_TRUE(myString.contains_just({0}));
}

TEST(StringTest, EmptyFile) {
  String myString;
  ASSERT_TRUE(myString.append_file_contents("StringCorpus/Empty.input"));
  ASSERT_EQ(myString.get_length(), 0);
  myString.replace_any_of({StringSlice::from_cstr(" "), StringSlice::from_cstr("\n")},
                          StringSlice::from_cstr(""));
  ASSERT_EQ(myString.get_length(), 0);
  ASSERT_EQ(myString.get_capacity(), 8);
}

TEST(StringTest, AlotOfHebrew) {
  String myString;
  ASSERT_TRUE(myString.append_file_contents("StringCorpus/AlotOfHebrew.input"));
  ASSERT_EQ(myString.get_length(), 12306);

  myString.replace_any_of({StringSlice::from_cstr(" "), StringSlice::from_cstr("\n")},
                          StringSlice::from_cstr(""));
//  fprintf(stderr, "[%s]\n", myString.get_c_str());
  ASSERT_EQ(myString.get_length(), 6102);
}

TEST(StringTest, NonAsciiChars) {
  String myString;
  ASSERT_TRUE(myString.append_file_contents("StringCorpus/NonAsciiChars.input"));
  ASSERT_EQ(myString.get_length(), 146);
  myString.replace_any_of({StringSlice::from_cstr(" ")}, StringSlice::from_cstr(""));
  ASSERT_EQ(myString.get_length(), 125);
}


