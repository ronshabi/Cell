// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include "Request.hpp"

#include "Encoding.hpp"
#include "Method.hpp"
#include "Uri.hpp"
#include "Version.hpp"
#include "cell/core/Charset.hpp"
#include "cell/core/Scanner.hpp"
#include "cell/core/String.hpp"
#include "cell/core/StringSlice.hpp"
#include "cell/log/Log.hpp"

namespace cell::http {

Request::Request(String *databuffer) noexcept : data_(databuffer) {}

RequestParserResult Request::Parse() noexcept {
  uint64_t cursor = 0;
  uint8_t ch;
  data_->RefreshLength();
  buf1.Clear();
  buf2.Clear();
  uri_.ClearDataBuffer();
  body_.Clear();

  //  CELL_LOG_DEBUG("Parsing request [%s]", data_->SubSlice(0, 1));

  while (cursor != data_->GetLen()) {
    ch = data_->ByteAt(cursor);

    switch (parser_state_) {
      case RequestParserState::NeedMethod: {
        if (IsWhitespace(ch)) {
          CELL_LOG_DEBUG("Method Buffer = '%s'", buf1.GetCString());
          method_ = HttpMethodFromString(buf1.SubSlice());

          if (method_ == Method::UnsupportedMethod) {
            return RequestParserResult::ErrorMethodInvalid;
          }

          parser_state_ = RequestParserState::NeedTarget;
          buf1.Clear();
          break;
        }

        buf1.AppendByte(ch);
        break;
      }

      case RequestParserState::NeedTarget: {
        if (IsWhitespace(ch)) {
          CELL_LOG_DEBUG("URI (Target) Buffer = '%s'",
                         uri_.GetDataBufferAsSlice().GetConstCharPtr());

          // Parse URI
          const auto result = uri_.Parse();

          if (result != UriParserResult::Ok) {
            return RequestParserResult::ErrorUriInvalid;
          }

          parser_state_ = RequestParserState::NeedVersion;
          break;
        }

        uri_.AppendByteToDataBuffer(ch);
        break;
      }

      case RequestParserState::NeedVersion: {
        if (ch == kCR) {
          CELL_LOG_DEBUG("Version Buffer = '%s'", buf1.GetCString());
          version_ = HttpVersionFromString(buf1.SubSlice());

          if (version_ == Version::UnsupportedVersion) {
            return RequestParserResult::ErrorVersionInvalid;
          }

          parser_state_ = RequestParserState::NeedCrlfAfterRequestLine;
          buf1.Clear();
          break;
        }

        buf1.AppendByte(ch);
        break;
      }

      case RequestParserState::NeedCrlfAfterRequestLine: {
        if (data_->ByteAt(cursor) != kLF) {
          return RequestParserResult::ErrorNoCrlfAfterRequestLine;
        }

        parser_state_ = RequestParserState::NeedHeaderKey;
        break;
      }

      case RequestParserState::NeedHeaderKey: {
        if (ch == kCR) {
          CELL_LOG_DEBUG_SIMPLE("No more headers, check last CRLF");
          parser_state_ = RequestParserState::NeedCrlfBetweenHeadersAndBody;
          buf1.Clear();
          break;
        }

        if (ch == ':') {
          buf1.ToLower();
          CELL_LOG_DEBUG("Found header key: '%s'", buf1.GetCString());
          parser_state_ = RequestParserState::EatingWhitespaceAfterHeaderKey;
          break;
        }

        // not CR, because we dealt with it in the first if
        if (IsWhitespace(ch)) {
          return RequestParserResult::ErrorFieldLineStartsWithWhitespace;
        }

        buf1.AppendByte(ch);
        break;
      }

      case RequestParserState::EatingWhitespaceAfterHeaderKey: {
        if (!IsWhitespace(ch)) {
          parser_state_ = RequestParserState::NeedHeaderValue;
          --cursor;
          break;
        }
        break;
      }

      case RequestParserState::NeedHeaderValue: {
        if (ch == kCR) {
          CELL_LOG_DEBUG("Header value: [%s]", buf2.GetCString());

          // Assign common headers
          // TODO: Extract to separate method
          if (buf1.Compare(StringSlice::FromCString("connection"))) {
            if (buf2.Compare(StringSlice::FromCString("keep-alive"))) {
              CELL_LOG_DEBUG_SIMPLE("[~] Connection: keep-alive");
              connection_ = Connection::KeepAlive;
            } else {
              CELL_LOG_DEBUG("[~] Connection defaults to close (actual value of header: %s)", buf2.GetCString());
              connection_ = Connection::Close;
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
          } else if (buf1.Compare(StringSlice::FromCString("accept-encoding"))) {
            CELL_LOG_DEBUG("[~] Passing accept-encoding string of '%s' to designated parser", buf1.GetCString());
            accept_encoding_ = encoding::ParseFromRequestHeader(buf2.SubSlice());

            if (accept_encoding_ == encoding::kErrorParsing) {
              CELL_LOG_DEBUG_SIMPLE("[!!!] Failed parsing accept-encoding, defaults to None");
              accept_encoding_ = 0;
            }
          } else {
            CELL_LOG_DEBUG("[~] Found uncommon header '%s' -> '%s', adding to table", buf1.GetCString(), buf2.GetCString());
            headers_.AddKeyValuePair(buf1, buf2);
          }

          buf1.Clear();
          buf2.Clear();
          parser_state_ = RequestParserState::NeedCrlfAfterHeaderValue;
          break;
        }

//        CELL_LOG_DEBUG("Header value::AppendByte [%c] (%X)", ch, ch);
        buf2.AppendByte(ch);
        break;
      }

      case RequestParserState::NeedCrlfAfterHeaderValue: {
//        CELL_LOG_DEBUG("RequestParserState::NeedCrlfAfterHeaderValue: Cursor at %02X", ch);

        if (data_->ByteAt(cursor) == kLF) {
          parser_state_ = RequestParserState::NeedHeaderKey;
          break ;
        }

        return RequestParserResult::ErrorNoCrlfAfterHeaderValue;
      }

      case RequestParserState::NeedCrlfBetweenHeadersAndBody: {
        // end of headers?
        if (data_->ByteAt(cursor) == kLF) {
          parser_state_ = RequestParserState::AppendingBody;
          break ;
        }

        return RequestParserResult::ErrorNoEndingCrlfBetweenHeadersAndBody;
      }

      case RequestParserState::AppendingBody: {
        if (method_ == Method::Head) {
          return RequestParserResult::ErrorHeadRequestBodyExists;
        }

//        CELL_LOG_DEBUG("Appending to body [%c] (%X)", ch, ch);
        body_.AppendByte(ch);
        break;
      }
    }

    ++cursor;
  }

  return RequestParserResult::Ok;
}

}  // namespace cell::http