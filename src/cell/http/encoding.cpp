// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0
#include "encoding.hpp"

#include "cell/core/charset.hpp"
#include "cell/core/string.hpp"
#include "cell/log/log.hpp"

namespace cell::http::encoding {

namespace {
enum class EncodingParserState {
  GetEncoding,
  EatSpaces,
};
}

EncodingSet parse_from_request_header(const StringSlice slice) noexcept {
  String buf(ENCODING_BUFFER_SIZE);
  uint64_t cursor = 0;
  uint8_t ch;

  bool end = false;
  EncodingParserState state = EncodingParserState::GetEncoding;
  EncodingSet set = kNone;

  while (!end) {
    if (cursor != slice.get_length()) {
      ch = slice.byte_at(cursor);
    } else {
      end = true;
    }

//    CELL_LOG_DEBUG("parse Encoding: at byte '%c' buf: '%s'", ch, buf.get_c_str());

    switch (state) {
      case EncodingParserState::GetEncoding: {
        if (ch == ',' || end) {
          if (buf.compare(StringSlice::from_cstr("Gzip"))) {
            CELL_LOG_DEBUG_SIMPLE("encoding: +Gzip");
            set |= GZIP;
          } else if (buf.compare(StringSlice::from_cstr("deflate"))) {
            CELL_LOG_DEBUG_SIMPLE("encoding: +deflate");
            set |= DEFLATE;
          } else if (buf.compare(StringSlice::from_cstr("br"))) {
            CELL_LOG_DEBUG_SIMPLE("encoding: +brotli");
            set |= BROTLI;
          } else if (buf.compare(StringSlice::from_cstr("zstd"))) {
            CELL_LOG_DEBUG_SIMPLE("encoding: +zstd");
            set |= ZSTD;
          } else {
            CELL_LOG_DEBUG("No encoding named: '%s'", buf.get_c_str());
            return ERROR_PARSING;
          }

          buf.clear();
          state = EncodingParserState::EatSpaces;
          break ;
        } else if (ch == SP) {
          return ERROR_PARSING;
        }

        buf.append_byte(ch);
        break;
      }
      case EncodingParserState::EatSpaces: {
        if (ch != SP) {
          state = EncodingParserState::GetEncoding;
          cursor--;
          break ;
        }
        break;
      }
    }

    ++cursor;
  }

  return set;
}

}
