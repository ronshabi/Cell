// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include "HttpRequest.hpp"

#include "HttpMethod.hpp"
#include "HttpUri.hpp"
#include "HttpVersion.hpp"
#include "cell/Charset.hpp"
#include "cell/Scanner.hpp"
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

  CELL_LOG_DEBUG("Parsing request [%s]", data_->GetCString());

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
        if (IsWhitespace(ch)) {
          return HttpRequestParserResult::ErrorFieldLineStartsWithWhitespace;
        }

        if (ch == ':') {
          buf1.ToLower();
          CELL_LOG_DEBUG("Found header key: '%s'", buf1.GetCString());
          parser_state_ = HttpRequestParserState::EatingWhitespaceAfterHeaderKey;
          break;
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
      }

      case HttpRequestParserState::NeedHeaderValue: {
        if (ch == kCR) {
          CELL_LOG_DEBUG("Header value: [%s]", buf2.GetCString());
          break;
        }
        CELL_LOG_DEBUG("Header value::AppendByte [%c]", ch);

        buf2.AppendByte(ch);
        break;
      }
      case HttpRequestParserState::NeedCrlfAfterHeaderValue: {
        break;
      }
      case HttpRequestParserState::AppendingBody: {
        break;
      }
    }

    ++cursor;
  }

  return HttpRequestParserResult::Ok;
}

}  // namespace cell::http