// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_HTTPENCODING_HPP
#define CELL_HTTPENCODING_HPP

#include <cstdint>

namespace cell::http {

enum HttpEncoding : uint64_t {
  None = 0b1,
  Deflate = 0b10,
  Gzip = 0b100,
  Brotli = 0b1000,
  ZStd = 0b10000
};

}

#endif  // CELL_HTTPENCODING_HPP
