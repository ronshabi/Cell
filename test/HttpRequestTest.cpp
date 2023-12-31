// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <cell/core/string.hpp>
#include <cell/core/string_slice.hpp>
#include <cell/http/request.hpp>

using namespace cell;
using cell::http::Request;

TEST(HttpRequestTest, Head1) {
  cell::String buf;
  Request request(&buf);
  buf.append_slice(StringSlice::from_cstr("HEAD /index.php?query=all HTTP/1.1\r\n"));

  const auto result = request.parse();

  ASSERT_EQ(result, http::RequestParserResult::Ok);
  ASSERT_STREQ(cell::http::method_to_string(request.get_method()).get_const_char_ptr(), "HEAD");
  ASSERT_STREQ(cell::http::version_to_string(request.get_version()).get_const_char_ptr(), "HTTP/1.1");
  ASSERT_STREQ(request.get_target().get_const_char_ptr(), "/index.php?query=all");
}

TEST(HttpRequestTest, Head2) {
  cell::String buf;
  Request request(&buf);
  buf.append_slice(
      StringSlice::from_cstr("HEAD / HTTP/3\r\n"
                             "Connection: keep-alive\r\n"
                             "\r\n"));

  const auto result = request.parse();

  ASSERT_EQ(result, http::RequestParserResult::Ok);
  ASSERT_STREQ(cell::http::method_to_string(request.get_method()).get_const_char_ptr(), "HEAD");
  ASSERT_STREQ(cell::http::version_to_string(request.get_version()).get_const_char_ptr(), "HTTP/3");
  ASSERT_STREQ(request.get_target().get_const_char_ptr(), "/");
  ASSERT_EQ(request.get_connection_type(), http::Connection::KeepAlive);
}

TEST(HttpRequestTest, Head3) {
  cell::String buf;
  Request request(&buf);
  buf.append_slice(
      StringSlice::from_cstr("HEAD /script.js?query=true HTTP/2\r\n"
                             "Connection: keep-alive\r\n"
                             "User-Agent: Mozilla/5.0 (Linux; Android 13;) AppleWebKit/537.36 "
                             "(KHTML, like Gecko) Version/4.0 Chrome/107.0.5304.141\r\n"
                             "Upgrade-Insecure-Requests:1\r\n"
                             "Host: www.example.com\r\n"
                             "Referrer: www.google.com\r\n"
                             "Sec-Fetch-Dest: script\r\n"
                             "Accept-Encoding: Gzip, deflate, br\r\n"
                             "\r\n"));

  const auto result = request.parse();

  ASSERT_EQ(result, http::RequestParserResult::Ok);
  ASSERT_STREQ(cell::http::method_to_string(request.get_method()).get_const_char_ptr(), "HEAD");
  ASSERT_STREQ(cell::http::version_to_string(request.get_version()).get_const_char_ptr(), "HTTP/2");
  ASSERT_STREQ(request.get_target().get_const_char_ptr(), "/script.js?query=true");

  ASSERT_EQ(request.get_connection_type(), http::Connection::KeepAlive);
  ASSERT_STREQ(request.get_user_agent().get_const_char_ptr(),
               "Mozilla/5.0 (Linux; Android 13;) AppleWebKit/537.36 (KHTML, like Gecko) "
               "Version/4.0 Chrome/107.0.5304.141");
  ASSERT_STREQ(request.get_host().get_const_char_ptr(), "www.example.com");
  ASSERT_STREQ(request.get_referrer().get_const_char_ptr(), "www.google.com");
  ASSERT_EQ(request.get_accept_encoding(),
            http::encoding::GZIP | http::encoding::DEFLATE | http::encoding::BROTLI);
  ASSERT_EQ(request.get_can_upgrade_insecure_connections(), true);
}

TEST(HttpRequestTest, Post1) {
  cell::String buf;
  Request request(&buf);
  buf.append_slice(
      StringSlice::from_cstr("POST /index.php HTTP/1.1\r\n"
                             "Connection: keep-alive\r\n"
                             "User-Agent: Mozilla/5.0 (Linux; Android 13;) AppleWebKit/537.36 "
                             "(KHTML, like Gecko) Version/4.0 Chrome/107.0.5304.141\r\n"
                             "Upgrade-Insecure-Requests:1\r\n"
                             "Host: www.example.com\r\n"
                             "Referrer: www.google.com\r\n"
                             "Sec-Fetch-Dest: script\r\n"
                             "Accept-Encoding: Gzip, deflate, br\r\n"
                             "\r\n"
                             "Post Request Body"));

  const auto result = request.parse();

  ASSERT_EQ(result, http::RequestParserResult::Ok);
  ASSERT_STREQ(cell::http::method_to_string(request.get_method()).get_const_char_ptr(), "POST");
  ASSERT_STREQ(cell::http::version_to_string(request.get_version()).get_const_char_ptr(), "HTTP/1.1");
  ASSERT_STREQ(request.get_target().get_const_char_ptr(), "/index.php");
  ASSERT_EQ(request.get_connection_type(), http::Connection::KeepAlive);
  ASSERT_STREQ(request.get_user_agent().get_const_char_ptr(),
               "Mozilla/5.0 (Linux; Android 13;) AppleWebKit/537.36 (KHTML, like Gecko) "
               "Version/4.0 Chrome/107.0.5304.141");
  ASSERT_STREQ(request.get_host().get_const_char_ptr(), "www.example.com");
  ASSERT_STREQ(request.get_referrer().get_const_char_ptr(), "www.google.com");
  ASSERT_EQ(request.get_accept_encoding(),
            http::encoding::GZIP | http::encoding::DEFLATE | http::encoding::BROTLI);
  ASSERT_EQ(request.get_can_upgrade_insecure_connections(), true);
  ASSERT_STREQ(request.get_body().get_const_char_ptr(), "Post Request Body");
}
