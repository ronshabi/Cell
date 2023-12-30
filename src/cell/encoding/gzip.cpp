// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include "gzip.hpp"

#include <zlib.h>

#include "cell/core/memory.hpp"
#include "cell/core/string_slice.hpp"
#include "cell/core/types.hpp"
#include "cell/log/log.hpp"

namespace cell::encoding {

Gzip::Gzip() {
  mem_zero(&m_stream, sizeof(z_stream));
  m_status = deflateInit2(&m_stream, DEFAULT_COMPRESSION_LEVEL, DEFAULT_METHOD,
                          DEFAULT_GZIP_WINDOW_BITS, DEFAULT_MEM_LEVEL, DEFAULT_STRATEGY);
}

Gzip::~Gzip() { deflateEnd(&m_stream); }

void Gzip::reset() { deflateReset(&m_stream); }


void Gzip::compress_string(const String& in, String& out, int block_size) {
  out.expand(block_size); // reserve block size if it isn't already that big
  m_stream.next_in = in.get_buffer_ptr();
  m_stream.avail_in = in.get_length();
  m_stream.next_out = out.get_buffer_ptr();
  m_stream.avail_out = out.get_capacity();

  CELL_ASSERT(m_stream.avail_out != 0);
  CELL_ASSERT(is_ok());

  constexpr auto flush = Z_FINISH; // we already have all the input we need
  int deflate_status;

  do {
    CELL_LOG_DEBUG_SIMPLE("DEFLATING");
    deflate_status = deflate(&m_stream, flush);
    CELL_ASSERT(deflate_status != Z_STREAM_ERROR);
  } while (m_stream.avail_out == 0);

  // finished
  deflateEnd(&m_stream);
  log::hexdump(reinterpret_cast<const u8*>(out.get_buffer_ptr()), 32);
}
StringSlice Gzip::get_status_as_string() const noexcept {
  switch (m_status) {
    case Z_OK:
      return StringSlice::from_cstr("ok");
    case Z_MEM_ERROR:
      return StringSlice::from_cstr("MEM_ERROR: not enough memory");
    case Z_STREAM_ERROR:
      return StringSlice::from_cstr("STREAM_ERROR: parameter is invalid");
    case Z_VERSION_ERROR:
      return StringSlice::from_cstr("VERSION_ERROR: zlib version isn't compatible");
    default:
      return StringSlice::from_cstr("unknown / unimplement zlib error");
  }
}

}  // namespace cell::encoding
