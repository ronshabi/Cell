// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_GZIP_HPP
#define CELL_GZIP_HPP

#include <zlib.h>

#include "cell/core/string.hpp"

namespace cell::encoding {
class Gzip {
 public:
  static constexpr int kCompressionLevel = Z_BEST_COMPRESSION;
  static constexpr int kDeflateMethod = Z_DEFLATED;
  static constexpr int kWindowBits = 15 + 16;
  static constexpr int kMemLevel = 9;
  static constexpr int kStrategy = Z_DEFAULT_STRATEGY;

  Gzip();
  ~Gzip();


  bool IsOk() const { return status_ == Z_OK; }
  void Reset();

  void CompressString(const String& in, String& out);

 private:
  int status_;
  z_stream stream_{};

};



}

#endif  // CELL_GZIP_HPP
