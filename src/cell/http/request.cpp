// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include "request.hpp"

#include "cell/core/charset.hpp"
#include "cell/core/scanner.hpp"
#include "cell/core/string.hpp"
#include "cell/core/string_slice.hpp"
#include "cell/log/Log.hpp"
#include "encoding.hpp"
#include "method.hpp"
#include "uri.hpp"
#include "version.hpp"

namespace cell::http {

Request::Request(String *databuffer) noexcept : m_data(databuffer) {}

RequestParserResult Request::parse() noexcept {
  uint64_t cursor = 0;
  uint8_t ch;
  m_data->refresh_length();
  m_buf1.clear();
  m_buf2.clear();
  m_uri.clear_data_buffer();
  m_body.clear();

  //  CELL_LOG_DEBUG("Parsing request [%s]", m_data->slice(0, 1));

  while (cursor != m_data->get_length()) {
    ch = m_data->byte_at(cursor);

    switch (m_parser_state) {
      case RequestParserState::NeedMethod: {
        if (is_whitespace(ch)) {
          CELL_LOG_DEBUG("Method Buffer = '%s'", m_buf1.get_c_str());
          m_method = method_from_string(m_buf1.slice());

          if (m_method == Method::UnsupportedMethod) {
            return RequestParserResult::ErrorMethodInvalid;
          }

          m_parser_state = RequestParserState::NeedTarget;
          m_buf1.clear();
          break;
        }

        m_buf1.append_byte(ch);
        break;
      }

      case RequestParserState::NeedTarget: {
        if (is_whitespace(ch)) {
          CELL_LOG_DEBUG("URI (Target) Buffer = '%s'",
                         m_uri.get_data_buffer_as_slice().GetConstCharPtr());

          // parse URI
          const auto result = m_uri.parse();

          if (result != UriParserResult::Ok) {
            return RequestParserResult::ErrorUriInvalid;
          }

          m_parser_state = RequestParserState::NeedVersion;
          break;
        }

        m_uri.append_byte_to_data_buffer(ch);
        break;
      }

      case RequestParserState::NeedVersion: {
        if (ch == CR) {
          CELL_LOG_DEBUG("Version Buffer = '%s'", m_buf1.get_c_str());
          m_version = version_from_string(m_buf1.slice());

          if (m_version == Version::UnsupportedVersion) {
            return RequestParserResult::ErrorVersionInvalid;
          }

          m_parser_state = RequestParserState::NeedCrlfAfterRequestLine;
          m_buf1.clear();
          break;
        }

        m_buf1.append_byte(ch);
        break;
      }

      case RequestParserState::NeedCrlfAfterRequestLine: {
        if (m_data->byte_at(cursor) != LF) {
          return RequestParserResult::ErrorNoCrlfAfterRequestLine;
        }

        m_parser_state = RequestParserState::NeedHeaderKey;
        break;
      }

      case RequestParserState::NeedHeaderKey: {
        if (ch == CR) {
          CELL_LOG_DEBUG_SIMPLE("No more headers, check last CRLF");
          m_parser_state = RequestParserState::NeedCrlfBetweenHeadersAndBody;
          m_buf1.clear();
          break;
        }

        if (ch == ':') {
          m_buf1.to_lower();
          CELL_LOG_DEBUG("Found header key: '%s'", m_buf1.get_c_str());
          m_parser_state = RequestParserState::EatingWhitespaceAfterHeaderKey;
          break;
        }

        // not CR, because we dealt with it in the first if
        if (is_whitespace(ch)) {
          return RequestParserResult::ErrorFieldLineStartsWithWhitespace;
        }

        m_buf1.append_byte(ch);
        break;
      }

      case RequestParserState::EatingWhitespaceAfterHeaderKey: {
        if (!is_whitespace(ch)) {
          m_parser_state = RequestParserState::NeedHeaderValue;
          --cursor;
          break;
        }
        break;
      }

      case RequestParserState::NeedHeaderValue: {
        if (ch == CR) {
          CELL_LOG_DEBUG("Header value: [%s]", m_buf2.get_c_str());

          // Assign common headers
          // TODO: Extract to separate method
          if (m_buf1.compare(StringSlice::from_cstr("connection"))) {
            if (m_buf2.compare(StringSlice::from_cstr("keep-alive"))) {
              CELL_LOG_DEBUG_SIMPLE("[~] Connection: keep-alive");
              m_connection = Connection::KeepAlive;
            } else {
              CELL_LOG_DEBUG("[~] Connection defaults to close (actual value of header: %s)",
                             m_buf2.get_c_str());
              m_connection = Connection::Close;
            }
          } else if (m_buf1.compare(StringSlice::from_cstr("host"))) {
            CELL_LOG_DEBUG("[~] Setting request host to '%s'", m_buf2.get_c_str());
            m_host = m_buf2;
          } else if (m_buf1.compare(StringSlice::from_cstr("referrer"))) {
            CELL_LOG_DEBUG("[~] Setting request referrer to '%s'", m_buf2.get_c_str());
            m_referrer = m_buf2;
          } else if (m_buf1.compare(StringSlice::from_cstr("user-agent"))) {
            CELL_LOG_DEBUG("[~] Setting request user-agent to '%s'", m_buf2.get_c_str());
            m_user_agent = m_buf2;
          } else if (m_buf1.compare(StringSlice::from_cstr("upgrade-insecure-requests"))) {
            if (m_buf2.compare(StringSlice::from_cstr("1"))) {
              CELL_LOG_DEBUG_SIMPLE("[~] Setting upgrade-insecure-requests to true");
              m_upgrade_insecure_requests = true;
            }
          } else if (m_buf1.compare(StringSlice::from_cstr("accept-encoding"))) {
            CELL_LOG_DEBUG("[~] Passing accept-encoding string of '%s' to designated parser",
                           m_buf1.get_c_str());
            m_accept_encoding = encoding::parse_from_request_header(m_buf2.slice());

            if (m_accept_encoding == encoding::ERROR_PARSING) {
              CELL_LOG_DEBUG_SIMPLE("[!!!] Failed parsing accept-encoding, defaults to None");
              m_accept_encoding = 0;
            }
          } else {
            CELL_LOG_DEBUG("[~] Found uncommon header '%s' -> '%s', adding to table",
                           m_buf1.get_c_str(), m_buf2.get_c_str());
            m_headers.AddKeyValuePair(m_buf1, m_buf2);
          }

          m_buf1.clear();
          m_buf2.clear();
          m_parser_state = RequestParserState::NeedCrlfAfterHeaderValue;
          break;
        }

//        CELL_LOG_DEBUG("Header value::append_byte [%c] (%X)", ch, ch);
        m_buf2.append_byte(ch);
        break;
      }

      case RequestParserState::NeedCrlfAfterHeaderValue: {
//        CELL_LOG_DEBUG("RequestParserState::NeedCrlfAfterHeaderValue: Cursor at %02X", ch);

        if (m_data->byte_at(cursor) == LF) {
          m_parser_state = RequestParserState::NeedHeaderKey;
          break ;
        }

        return RequestParserResult::ErrorNoCrlfAfterHeaderValue;
      }

      case RequestParserState::NeedCrlfBetweenHeadersAndBody: {
        // end of headers?
        if (m_data->byte_at(cursor) == LF) {
          m_parser_state = RequestParserState::AppendingBody;
          break ;
        }

        return RequestParserResult::ErrorNoEndingCrlfBetweenHeadersAndBody;
      }

      case RequestParserState::AppendingBody: {
        if (m_method == Method::Head) {
          return RequestParserResult::ErrorHeadRequestBodyExists;
        }

//        CELL_LOG_DEBUG("Appending to body [%c] (%X)", ch, ch);
        m_body.append_byte(ch);
        break;
      }
    }

    ++cursor;
  }

  return RequestParserResult::Ok;
}

}  // namespace cell::http