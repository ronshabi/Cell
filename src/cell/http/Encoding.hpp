// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_ENCODING_HPP
#define CELL_ENCODING_HPP

#include <cstdint>

namespace cell::http {

enum Encoding : uint64_t {
  None = 0b1,
  Deflate = 0b10,
  Gzip = 0b100,
  Brotli = 0b1000,
  ZStd = 0b10000
};

}

#endif  // CELL_ENCODING_HPP
