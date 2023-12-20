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

HttpRequestParsingStatus HttpRequest::Parse() noexcept {
  data_->RefreshLength();
  parse_buffer_.Clear();
  scanner_.ResetState();
  CELL_LOG_DEBUG("Parsing request [%s]", data_->GetCString());

  // Parse method
  scanner_.AppendToBufferUntilHittingChar(parse_buffer_, ' ');
  method_ = HttpMethodFromString(parse_buffer_.SubSlice());
  if (method_ == HttpMethod::UnsupportedMethod) {
    return HttpRequestParsingStatus::ErrorMethodInvalid;
  }
  CELL_LOG_DEBUG("Method: [%s]", parse_buffer_.GetCString());

  // Parse URI
  scanner_.AppendToBufferUntilHittingChar(uri_buffer_, ' ');
  CELL_LOG_DEBUG("URI: [%s]", uri_buffer_.GetCString());

  // Parse Version
  parse_buffer_.Clear();
  scanner_.AppendToBufferUntilHittingChar(parse_buffer_, kCR);

  version_ = HttpVersionFromString(parse_buffer_.SubSlice());
  if (version_ == HttpVersion::HttpUnsupportedVersion) {
    return HttpRequestParsingStatus::ErrorInvalidHttpVersion;
  }
  CELL_LOG_DEBUG("Version: [%s]", HttpVersionToString(version_).GetConstCharPtr());

  if (scanner_.Peek() != kCR && !scanner_.AdvanceContinuousExactly(kLF)) {
    return HttpRequestParsingStatus::ErrorInvalidRequest;
  }

  // On CRLF?
  if (scanner_.Peek() != kCR && !scanner_.AdvanceContinuousExactly(kLF)) {
    return HttpRequestParsingStatus::ErrorInvalidRequest;
  }

  //  parse_buffer_.Clear();
  //  scanner_.AppendToBufferUntilHittingChar(parse_buffer_, ':');
  //  CELL_LOG_DEBUG("Header: [%s]", parse_buffer_.GetCString());

  return HttpRequestParsingStatus::Ok;
}

}  // namespace cell::http