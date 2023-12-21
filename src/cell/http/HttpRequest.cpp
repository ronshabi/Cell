// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include "HttpRequest.hpp"

#include "HttpMethod.hpp"
#include "HttpUri.hpp"
#include "HttpVersion.hpp"
#include "cell/Charset.hpp"
#include "cell/Scanner.hpp"
#include "cell/String.hpp"
#include "cell/StringSlice.hpp"
#include "cell/log/Log.hpp"

namespace cell::http {

HttpRequest::HttpRequest(String *databuffer) noexcept : data_(databuffer) {}

HttpRequestParserResult HttpRequest::Parse() noexcept {
  uint64_t cursor = 0;
  uint8_t ch;
  data_->RefreshLength();
  buf1.Clear();
  buf2.Clear();

//  CELL_LOG_DEBUG("Parsing request [%s]", data_->SubSlice(0, 1));

  while (cursor != data_->GetLen()) {
    ch = data_->CharAt(cursor);

    switch (parser_state_) {
      case HttpRequestParserState::NeedMethod: {
        if (IsWhitespace(ch)) {
          CELL_LOG_DEBUG("Method Buffer = '%s'", buf1.GetCString());
          method_ = HttpMethodFromString(buf1.SubSlice());

          if (method_ == HttpMethod::UnsupportedMethod) {
            return HttpRequestParserResult::ErrorMethodInvalid;
          }

          parser_state_ = HttpRequestParserState::NeedTarget;
          buf1.Clear();
          break ;
        }

        buf1.AppendByte(ch);
        break;
      }

      case HttpRequestParserState::NeedTarget: {
        if (IsWhitespace(ch)) {
          CELL_LOG_DEBUG("URI (Target) Buffer = '%s'", buf1.GetCString());

          uri_.SetBufferContents(buf1);
          parser_state_ = HttpRequestParserState::NeedVersion;
          buf1.Clear();
          break ;
        }

        buf1.AppendByte(ch);
        break;
      }

      case HttpRequestParserState::NeedVersion: {
        if (ch == kCR) {
          CELL_LOG_DEBUG("Version Buffer = '%s'", buf1.GetCString());
          version_ = HttpVersionFromString(buf1.SubSlice());

          if (version_ == HttpVersion::UnsupportedVersion) {
            return HttpRequestParserResult::ErrorVersionInvalid;
          }

          parser_state_ = HttpRequestParserState::NeedCrlfAfterRequestLine;
          buf1.Clear();
          break ;
        }

        buf1.AppendByte(ch);
        break;
      }

      case HttpRequestParserState::NeedCrlfAfterRequestLine: {
        if (data_->CharAt(cursor) != kLF) {
          return HttpRequestParserResult::ErrorNoCrlfAfterRequestLine;
        }

        parser_state_ = HttpRequestParserState::NeedHeaderKey;
        break;
      }

      case HttpRequestParserState::NeedHeaderKey: {
        if (ch == kCR) {
          CELL_LOG_DEBUG_SIMPLE("No more headers, check last CRLF");
          parser_state_ = HttpRequestParserState::NeedCrlfBetweenHeadersAndBody;
          buf1.Clear();
          break;
        }

        if (ch == ':') {
          buf1.ToLower();
          CELL_LOG_DEBUG("Found header key: '%s'", buf1.GetCString());
          parser_state_ = HttpRequestParserState::EatingWhitespaceAfterHeaderKey;
          break;
        }

        // not CR, because we dealt with it in the first if
        if (IsWhitespace(ch)) {
          return HttpRequestParserResult::ErrorFieldLineStartsWithWhitespace;
        }

        buf1.AppendByte(ch);
        break;
      }

      case HttpRequestParserState::EatingWhitespaceAfterHeaderKey: {
        if (!IsWhitespace(ch)) {
          parser_state_ = HttpRequestParserState::NeedHeaderValue;
          --cursor;
          break;
        }
        break ;
      }

      case HttpRequestParserState::NeedHeaderValue: {
        if (ch == kCR) {
          CELL_LOG_DEBUG("Header value: [%s]", buf2.GetCString());

          // Assign common headers
          // TODO: Extract to separate method
          if (buf1.Compare(StringSlice::FromCString("connection"))) {
            if (buf2.Compare(StringSlice::FromCString("keep-alive"))) {
              CELL_LOG_DEBUG_SIMPLE("[~] Connection: keep-alive");
              connection_ = HttpConnection::KeepAlive;
            } else {
              CELL_LOG_DEBUG("[~] Connection defaults to close (actual value of header: %s)", buf2.GetCString());
              connection_ = HttpConnection::Close;
            }
          } else if (buf1.Compare(StringSlice::FromCString("host"))) {
            CELL_LOG_DEBUG("[~] Setting request host to '%s'", buf2.GetCString());
            host_ = buf2;
          } else if (buf1.Compare(StringSlice::FromCString("referrer"))) {
            CELL_LOG_DEBUG("[~] Setting request referrer to '%s'", buf2.GetCString());
            referrer_ = buf2;
          } else if (buf1.Compare(StringSlice::FromCString("user-agent"))) {
            CELL_LOG_DEBUG("[~] Setting request user-agent to '%s'", buf2.GetCString());
            user_agent_ = buf2;
          } else if (buf1.Compare(StringSlice::FromCString("upgrade-insecure-requests"))) {
            if (buf2.Compare(StringSlice::FromCString("1"))) {
              CELL_LOG_DEBUG_SIMPLE("[~] Setting upgrade-insecure-requests to true");
              upgrade_insecure_requests_ = true;
            }
          } else {
            CELL_LOG_DEBUG("[~] Found uncommon header '%s' -> '%s', adding to table", buf1.GetCString(), buf2.GetCString());
            headers_.AddKeyValuePair(buf1, buf2);
          }

          buf1.Clear();
          buf2.Clear();
          parser_state_ = HttpRequestParserState::NeedCrlfAfterHeaderValue;
          break;
        }

//        CELL_LOG_DEBUG("Header value::AppendByte [%c] (%X)", ch, ch);
        buf2.AppendByte(ch);
        break;
      }

      case HttpRequestParserState::NeedCrlfAfterHeaderValue: {
//        CELL_LOG_DEBUG("HttpRequestParserState::NeedCrlfAfterHeaderValue: Cursor at %02X", ch);

        if (data_->CharAt(cursor) == kLF) {
          parser_state_ = HttpRequestParserState::NeedHeaderKey;
          break ;
        }

        return HttpRequestParserResult::ErrorNoCrlfAfterHeaderValue;
      }

      case HttpRequestParserState::NeedCrlfBetweenHeadersAndBody: {
        // end of headers?
        if (data_->CharAt(cursor) == kLF) {
          parser_state_ = HttpRequestParserState::AppendingBody;
          break ;
        }

        return HttpRequestParserResult::ErrorNoEndingCrlfBetweenHeadersAndBody;
      }

      case HttpRequestParserState::AppendingBody: {
        if (method_ == HttpMethod::Head) {
          return HttpRequestParserResult::ErrorHeadRequestBodyExists;
        }

        CELL_LOG_DEBUG("Appending to body [%c] (%X)", ch, ch);
        buf1.AppendByte(ch);
        break;
      }
    }

    ++cursor;
  }

  return HttpRequestParserResult::Ok;
}

}  // namespace cell::http