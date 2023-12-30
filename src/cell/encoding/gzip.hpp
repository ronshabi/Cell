// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_GZIP_HPP
#define CELL_GZIP_HPP

#include <zlib.h>

#include "cell/core/string.hpp"
#include "cell/core/string_slice.hpp"

namespace cell::encoding {
class Gzip {
 public:
  static constexpr int DEFAULT_COMPRESSION_LEVEL = Z_BEST_COMPRESSION;
  static constexpr int DEFAULT_METHOD = Z_DEFLATED;
  static constexpr int DEFAULT_GZIP_WINDOW_BITS = 15 + 16;
  static constexpr int DEFAULT_MEM_LEVEL = 9;
  static constexpr int DEFAULT_STRATEGY = Z_DEFAULT_STRATEGY;

  Gzip();
  ~Gzip();


  [[nodiscard]] bool is_ok() const
  { return m_status == Z_OK; }

  [[nodiscard]] StringSlice get_status_as_string() const noexcept;

  void reset();

  void compress_string(const String& in, String& out, int block_size = 512);

 private:
  int m_status;
  z_stream m_stream{};
};



}

#endif  // CELL_GZIP_HPP
