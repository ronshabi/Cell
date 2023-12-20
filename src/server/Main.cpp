// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include <cell/Scanner.hpp>
#include <cell/String.hpp>
#include <cell/StringSlice.hpp>
#include <cell/http/HttpRequest.hpp>
#include <iostream>

using namespace cell;

int main(int argc, char* argv[]) {
  String buf;
  http::HttpRequest request(&buf);
  buf.AppendStringSlice(StringSlice::FromCString("GET /index.php?query=all HTTP/1.1\r\n"));

  const auto result = request.Parse();
  if (result == cell::http::HttpRequestParsingStatus::Ok) {
    printf("Method: %s, Version: %s, Target: [%s]\n",
           HttpMethodToString(request.GetMethod()).GetConstCharPtr(),
           HttpVersionToString(request.GetVersion()).GetConstCharPtr(),
           request.GetTarget().GetConstCharPtr());
  } else {
    printf("Result not ok\n");
  }

  return 0;
}