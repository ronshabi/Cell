// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0
#include "Encoding.hpp"

#include "cell/core/Charset.hpp"
#include "cell/core/String.hpp"
#include "cell/log/Log.hpp"

namespace cell::http::encoding {

namespace {
enum class EncodingParserState {
  GetEncoding,
  EatSpaces,
};
}

EncodingSet ParseFromRequestHeader(const StringSlice slice) noexcept {
  String buf(kEncodingBufferSize);
  uint64_t cursor = 0;
  uint8_t ch;

  bool end = false;
  EncodingParserState state = EncodingParserState::GetEncoding;
  EncodingSet set = kNone;

  while (!end) {
    if (cursor != slice.GetLen()) {
      ch = slice.ByteAt(cursor);
    } else {
      end = true;
    }

//    CELL_LOG_DEBUG("Parse Encoding: at byte '%c' buf: '%s'", ch, buf.GetCString());

    switch (state) {
      case EncodingParserState::GetEncoding: {
        if (ch == ',' || end) {
          if (buf.Compare(StringSlice::FromCString("gzip"))) {
            CELL_LOG_DEBUG_SIMPLE("encoding: +gzip");
            set |= kGzip;
          } else if (buf.Compare(StringSlice::FromCString("deflate"))) {
            CELL_LOG_DEBUG_SIMPLE("encoding: +deflate");
            set |= kDeflate;
          } else if (buf.Compare(StringSlice::FromCString("br"))) {
            CELL_LOG_DEBUG_SIMPLE("encoding: +brotli");
            set |= kBrotli;
          } else if (buf.Compare(StringSlice::FromCString("zstd"))) {
            CELL_LOG_DEBUG_SIMPLE("encoding: +zstd");
            set |= kZstd;
          } else {
            CELL_LOG_DEBUG("No encoding named: '%s'", buf.GetCString());
            return kErrorParsing;
          }

          buf.Clear();
          state = EncodingParserState::EatSpaces;
          break ;
        } else if (ch == kSP) {
          return kErrorParsing;
        }

        buf.AppendByte(ch);
        break;
      }
      case EncodingParserState::EatSpaces: {
        if (ch != kSP) {
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
