// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include "Gzip.hpp"

#include <zlib.h>

#include "cell/core/memory.hpp"
#include "cell/log/Log.hpp"

namespace cell::encoding {
Gzip::Gzip() {
  mem_zero(&stream_, sizeof(z_stream));
  status_ = deflateInit2(&stream_, kCompressionLevel, kDeflateMethod, kWindowBits, kMemLevel, kStrategy);
}

Gzip::~Gzip() {
  deflateEnd(&stream_);
}

void Gzip::Reset() {
  deflateReset(&stream_);
}

void Gzip::CompressString(const String& in, String& out) {
  stream_.next_in = in.get_buffer_ptr();
  stream_.avail_in = in.get_length();
  stream_.next_out = out.get_buffer_ptr();
  stream_.avail_out = out.get_capacity();
}

}

