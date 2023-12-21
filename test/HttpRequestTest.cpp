// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <cell/http/HttpRequest.hpp>
#include "cell/String.hpp"
#include "cell/http/HttpMethod.hpp"

using namespace cell;
using cell::http::HttpRequest;

TEST(HttpRequestTest, Head1) {
  cell::String buf;
  HttpRequest request(&buf);
  buf.AppendStringSlice(StringSlice::FromCString("HEAD /index.php?query=all HTTP/1.1\r\n"));

  const auto result = request.Parse();

  ASSERT_EQ(result, http::HttpRequestParserResult::Ok);
  ASSERT_STREQ(cell::http::HttpMethodToString(request.GetMethod()).GetConstCharPtr(), "HEAD");
  ASSERT_STREQ(cell::http::HttpVersionToString(request.GetVersion()).GetConstCharPtr(), "HTTP/1.1");
  ASSERT_STREQ(request.GetTarget().GetConstCharPtr(), "/index.php?query=all");
}

TEST(HttpRequestTest, Head2) {
  cell::String buf;
  HttpRequest request(&buf);
  buf.AppendStringSlice(StringSlice::FromCString("HEAD / HTTP/3\r\n"
                                                 "Connection: keep-alive\r\n"));

  const auto result = request.Parse();

  ASSERT_EQ(result, http::HttpRequestParserResult::Ok);
  ASSERT_STREQ(cell::http::HttpMethodToString(request.GetMethod()).GetConstCharPtr(), "HEAD");
  ASSERT_STREQ(cell::http::HttpVersionToString(request.GetVersion()).GetConstCharPtr(), "HTTP/3");
  ASSERT_STREQ(request.GetTarget().GetConstCharPtr(), "/");
}