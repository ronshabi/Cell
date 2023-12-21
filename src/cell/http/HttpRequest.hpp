// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_HTTPREQUEST_HPP
#define CELL_HTTPREQUEST_HPP

#include "HttpConnection.hpp"
#include "HttpEncoding.hpp"
#include "HttpMethod.hpp"
#include "HttpUri.hpp"
#include "HttpVersion.hpp"
#include "cell/Scanner.hpp"
#include "cell/String.hpp"
#include "cell/StringSlice.hpp"
#include "cell/WeakStringCache.hpp"

namespace cell::http {

enum class HttpRequestParserResult {
  Ok,
  ErrorInvalidRequest,
  ErrorMethodInvalid,
  ErrorVersionInvalid,
  ErrorNoCrlfAfterRequestLine,
  ErrorNoCrlfAfterHeaderValue,
  ErrorNoEndingCrlfBetweenHeadersAndBody,
  ErrorUriTooLong,
  ErrorFieldLineStartsWithWhitespace,
  ErrorHeadRequestBodyExists,
};

enum class HttpRequestParserState {
  NeedMethod,
  NeedTarget,
  NeedVersion,
  NeedCrlfAfterRequestLine,
  NeedHeaderKey,
  EatingWhitespaceAfterHeaderKey,
  NeedHeaderValue,
  NeedCrlfAfterHeaderValue,
  NeedCrlfBetweenHeadersAndBody,
  AppendingBody,
};


class HttpRequest {
 public:
  explicit HttpRequest(String* databuffer) noexcept;

  [[nodiscard]] HttpRequestParserResult Parse() noexcept;

  [[nodiscard]] HttpVersion GetVersion() const noexcept { return version_; }
  [[nodiscard]] HttpMethod GetMethod() const noexcept { return method_; }
  [[nodiscard]] StringSlice GetTarget() const noexcept { return uri_.GetPath(); }
  [[nodiscard]] StringSlice GetUserAgent() const noexcept { return user_agent_.SubSlice(); }
  [[nodiscard]] StringSlice GetHost() const noexcept { return host_.SubSlice(); }
  [[nodiscard]] StringSlice GetReferrer() const noexcept { return referrer_.SubSlice(); }
  [[nodiscard]] HttpEncoding GetAcceptEncoding() const noexcept { return accept_encoding_; }
  [[nodiscard]] HttpConnection GetConnectionType() const noexcept { return connection_; }
  [[nodiscard]] bool CanUpgradeInsecureConnections() const noexcept {
    return upgrade_insecure_requests_;
  }

 private:
  static constexpr uint64_t kDefaultBufferCapacity = 4096;
  static constexpr uint64_t kDefaultHeaderValueCapacity = 4096;

  String* data_;
  String buf1{kDefaultBufferCapacity};
  String buf2{kDefaultBufferCapacity};

  HttpRequestParserState parser_state_{HttpRequestParserState::NeedMethod};


  HttpVersion version_{HttpVersion::UnsupportedVersion};
  HttpMethod method_{HttpMethod::UnsupportedMethod};
  HttpUri uri_{};
  HttpEncoding accept_encoding_{None};
  HttpConnection connection_{HttpConnection::Close};
  bool upgrade_insecure_requests_{false};
  String host_{kDefaultHeaderValueCapacity};
  String referrer_{kDefaultHeaderValueCapacity};
  String user_agent_{kDefaultHeaderValueCapacity};
  WeakStringCache headers_{};
};

}  // namespace cell::http

#endif  // CELL_HTTPREQUEST_HPP
