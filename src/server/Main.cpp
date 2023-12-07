// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include <cell/Scanner.hpp>
#include <cell/String.hpp>
#include <iostream>

using namespace cell;

int main(int argc, char* argv[]) {
  String request;
  request.AppendCString("GET /index.php?query=all HTTP/1.1");

  Scanner scanner(&request);

  while (true) {
    const auto c = scanner.GetNextChar();
    if (scanner.IsEof()) {
      break;
    }

    std::cout << "[" << c << "]";
  }

  return 0;
}