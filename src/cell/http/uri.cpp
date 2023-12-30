// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include "uri.hpp"

#include "cell/core/charset.hpp"
#include "cell/core/types.hpp"
#include "cell/log/log.hpp"

namespace cell::http {
UriParserResult Uri::parse() noexcept {
  uint64_t cursor = 0;
  uint8_t ch;
  bool end_flag = false;

  CELL_LOG_DEBUG("parse URI: [%s]", m_data.get_c_str());

  while (!end_flag) {
    if (cursor != m_data.get_length()) {
      ch = m_data.byte_at(cursor);
    } else {
      end_flag = true;
    }

    switch (state_) {
      case UriParserState::GetType: {
        if (ch == '/') {
          m_uri_type = UriType::Relative;
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

          CELL_LOG_DEBUG("URI Path: [%s]", m_path.get_c_str());

          if (!Uri::decode(m_path.slice(), m_path_decoded)) {
            CELL_LOG_DEBUG_SIMPLE("Could not decode URI path");
            return UriParserResult::DecodingPathFailed;
          }

          CELL_LOG_DEBUG("URI Path Decoded: [%s]", m_path_decoded.get_c_str());
          break;
        }

        m_path.append_byte(ch);
        break;
      }
      case UriParserState::GetQueryKey: {
        if (ch == '=') {
          state_ = UriParserState::GetQueryValue;
          break;
        }

        m_query_key.append_byte(ch);
        break;
      }
      case UriParserState::GetQueryValue: {
        if (ch == '&' || end_flag) {
          state_ = UriParserState::GetQueryKey;
          CELL_LOG_DEBUG("URI Query Key: [%s] -> [%s]", m_query_key.get_c_str(),
                         m_query_value.get_c_str());

          if (!Uri::decode(m_query_value.slice(), m_query_value_decoded)) {
            CELL_LOG_DEBUG_SIMPLE("URI Query Value decoding failed");
            return UriParserResult::DecodingQueryValueFailed;
          }

          CELL_LOG_DEBUG("URI Query Key Decoded: [%s] -> [%s]", m_query_key.get_c_str(),
                         m_query_value_decoded.get_c_str());

          m_queries.AddKeyValuePair(m_query_key, m_query_value_decoded);
          m_query_key.clear();
          m_query_value.clear();
          break;
        }

        m_query_value.append_byte(ch);
        break;
      }
    }

    ++cursor;
  }

  return UriParserResult::Ok;
}

bool Uri::decode(StringSlice slice, String& out) {
  u64 cursor = 0;
  u8 ch;
  bool fail_flag = false;

  while (cursor < slice.get_length()) {
    ch = slice.byte_at(cursor);

    if (ch == '%') {
      if (cursor + 2 >= slice.get_length()) {
        return false;
      }
      const auto decoded_byte =
          hex_pair_to_byte(slice.byte_at(cursor + 1), slice.byte_at(cursor + 2), fail_flag);
      if (fail_flag) {
        return false; // fail, '%' with 2 non-hexdigs
      }

      out.append_byte(decoded_byte);
      cursor += 2;
    } else {
      out.append_byte(ch);
    }

    ++cursor;
  }

  return true;
}
}  // namespace cell::http