// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <cell/http/Request.hpp>

#include "cell/core/String.hpp"
#include "cell/http/Connection.hpp"
#include "cell/http/Encoding.hpp"
#include "cell/http/Method.hpp"

using namespace cell;
using cell::http::Request;

TEST(HttpRequestTest, Head1) {
  cell::String buf;
  Request request(&buf);
  buf.AppendStringSlice(StringSlice::FromCString("HEAD /index.php?query=all HTTP/1.1\r\n"));

  const auto result = request.Parse();

  ASSERT_EQ(result, http::RequestParserResult::Ok);
  ASSERT_STREQ(cell::http::HttpMethodToString(request.GetMethod()).GetConstCharPtr(), "HEAD");
  ASSERT_STREQ(cell::http::HttpVersionToString(request.GetVersion()).GetConstCharPtr(), "HTTP/1.1");
  ASSERT_STREQ(request.GetTarget().GetConstCharPtr(), "/index.php?query=all");
}

TEST(HttpRequestTest, Head2) {
  cell::String buf;
  Request request(&buf);
  buf.AppendStringSlice(
      StringSlice::FromCString("HEAD / HTTP/3\r\n"
                               "Connection: keep-alive\r\n"
                               "\r\n"));

  const auto result = request.Parse();

  ASSERT_EQ(result, http::RequestParserResult::Ok);
  ASSERT_STREQ(cell::http::HttpMethodToString(request.GetMethod()).GetConstCharPtr(), "HEAD");
  ASSERT_STREQ(cell::http::HttpVersionToString(request.GetVersion()).GetConstCharPtr(), "HTTP/3");
  ASSERT_STREQ(request.GetTarget().GetConstCharPtr(), "/");
  ASSERT_EQ(request.GetConnectionType(), http::Connection::KeepAlive);
}

TEST(HttpRequestTest, Head3) {
  cell::String buf;
  Request request(&buf);
  buf.AppendStringSlice(
      StringSlice::FromCString("HEAD /script.js?query=true HTTP/2\r\n"
                               "Connection: keep-alive\r\n"
                               "User-Agent: Mozilla/5.0 (Linux; Android 13;) AppleWebKit/537.36 "
                               "(KHTML, like Gecko) Version/4.0 Chrome/107.0.5304.141\r\n"
                               "Upgrade-Insecure-Requests:1\r\n"
                               "Host: www.example.com\r\n"
                               "Referrer: www.google.com\r\n"
                               "Sec-Fetch-Dest: script\r\n"
                               "Accept-Encoding: gzip, deflate, br\r\n"
                               "\r\n"));

  const auto result = request.Parse();

  ASSERT_EQ(result, http::RequestParserResult::Ok);
  ASSERT_STREQ(cell::http::HttpMethodToString(request.GetMethod()).GetConstCharPtr(), "HEAD");
  ASSERT_STREQ(cell::http::HttpVersionToString(request.GetVersion()).GetConstCharPtr(), "HTTP/2");
  ASSERT_STREQ(request.GetTarget().GetConstCharPtr(), "/script.js?query=true");

  ASSERT_EQ(request.GetConnectionType(), http::Connection::KeepAlive);
  ASSERT_STREQ(request.GetUserAgent().GetConstCharPtr(),
               "Mozilla/5.0 (Linux; Android 13;) AppleWebKit/537.36 (KHTML, like Gecko) "
               "Version/4.0 Chrome/107.0.5304.141");
  ASSERT_STREQ(request.GetHost().GetConstCharPtr(), "www.example.com");
  ASSERT_STREQ(request.GetReferrer().GetConstCharPtr(), "www.google.com");
  ASSERT_EQ(request.GetAcceptEncoding(),
            http::encoding::kGzip | http::encoding::kDeflate | http::encoding::kBrotli);
  ASSERT_EQ(request.CanUpgradeInsecureConnections(), true);
}

TEST(HttpRequestTest, Post1) {
  cell::String buf;
  Request request(&buf);
  buf.AppendStringSlice(
      StringSlice::FromCString("POST /index.php HTTP/1.1\r\n"
                               "Connection: keep-alive\r\n"
                               "User-Agent: Mozilla/5.0 (Linux; Android 13;) AppleWebKit/537.36 "
                               "(KHTML, like Gecko) Version/4.0 Chrome/107.0.5304.141\r\n"
                               "Upgrade-Insecure-Requests:1\r\n"
                               "Host: www.example.com\r\n"
                               "Referrer: www.google.com\r\n"
                               "Sec-Fetch-Dest: script\r\n"
                               "Accept-Encoding: gzip, deflate, br\r\n"
                               "\r\n"
                               "Post Request Body"));

  const auto result = request.Parse();

  ASSERT_EQ(result, http::RequestParserResult::Ok);
  ASSERT_STREQ(cell::http::HttpMethodToString(request.GetMethod()).GetConstCharPtr(), "POST");
  ASSERT_STREQ(cell::http::HttpVersionToString(request.GetVersion()).GetConstCharPtr(), "HTTP/1.1");
  ASSERT_STREQ(request.GetTarget().GetConstCharPtr(), "/index.php");
  ASSERT_EQ(request.GetConnectionType(), http::Connection::KeepAlive);
  ASSERT_STREQ(request.GetUserAgent().GetConstCharPtr(),
               "Mozilla/5.0 (Linux; Android 13;) AppleWebKit/537.36 (KHTML, like Gecko) "
               "Version/4.0 Chrome/107.0.5304.141");
  ASSERT_STREQ(request.GetHost().GetConstCharPtr(), "www.example.com");
  ASSERT_STREQ(request.GetReferrer().GetConstCharPtr(), "www.google.com");
  ASSERT_EQ(request.GetAcceptEncoding(),
            http::encoding::kGzip | http::encoding::kDeflate | http::encoding::kBrotli);
  ASSERT_EQ(request.CanUpgradeInsecureConnections(), true);
  ASSERT_STREQ(request.GetBody().GetConstCharPtr(), "Post Request Body");
}
