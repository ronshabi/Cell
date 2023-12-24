// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include "Uri.hpp"

#include "cell/core/Charset.hpp"
#include "cell/core/Types.hpp"
#include "cell/log/Log.hpp"

namespace cell::http {
UriParserResult Uri::Parse() noexcept {
  uint64_t cursor = 0;
  uint8_t ch;
  bool end_flag = false;

  CELL_LOG_DEBUG("Parse URI: [%s]", data_.GetCString());

  while (!end_flag) {
    if (cursor != data_.GetLen()) {
      ch = data_.ByteAt(cursor);
    } else {
      end_flag = true;
    }

    switch (state_) {
      case UriParserState::GetType: {
        if (ch == '/') {
          uri_type_ = UriType::Relative;
          state_ = UriParserState::GetPath;
          CELL_LOG_DEBUG_SIMPLE("Uri type: Relative");
          break;
        }

        CELL_LOG_DEBUG_SIMPLE("Uri type is Absolute, but we don't support it yet (TODO)");
        return UriParserResult::UnsupportedUriType;
      }
      case UriParserState::GetPath: {
        if (ch == '?') {
          state_ = UriParserState::GetQueryKey;
          CELL_LOG_DEBUG("URI Path: [%s]", path_.GetCString());
          break;
        }

        path_.AppendByte(ch);
        break;
      }
      case UriParserState::GetQueryKey: {
        if (ch == '=') {
          state_ = UriParserState::GetQueryValue;
          break;
        }

        query_key_.AppendByte(ch);
        break;
      }
      case UriParserState::GetQueryValue: {
        if (ch == '&' || end_flag) {
          state_ = UriParserState::GetQueryKey;
          CELL_LOG_DEBUG("URI Query Key: [%s] -> [%s]", query_key_.GetCString(),
                         query_value_.GetCString());
          queries_.AddKeyValuePair(query_key_, query_value_);
          query_key_.Clear();
          query_value_.Clear();
          break;
        }

        query_value_.AppendByte(ch);
        break;
      }
    }

    ++cursor;
  }

  return UriParserResult::Ok;
}

bool Uri::Decode(const StringSlice slice, String& out) {
  u64 cursor = 0;
  u8 ch;
  bool fail_flag = false;

  while (cursor < slice.GetLen()) {
    ch = slice.ByteAt(cursor);

    if (ch == '%') {
      if (cursor + 2 >= slice.GetLen()) {
        return false;
      }
      const auto decoded_byte = HexPairToByte(slice.ByteAt(cursor + 1), slice.ByteAt(cursor + 2), fail_flag);
      if (fail_flag) {
        return false; // fail, '%' with 2 non-hexdigs
      }

      out.AppendByte(decoded_byte);
      cursor += 2;
    } else {
      out.AppendByte(ch);
    }

    ++cursor;
  }

  return true;
}
}  // namespace cell::http