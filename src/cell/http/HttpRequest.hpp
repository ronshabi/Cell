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

namespace cell::http {

enum class HttpRequestParsingStatus {
  Ok,
  ErrorMethodInvalid,
  ErrorInvalidRequest,
  ErrorInvalidHttpVersion,
  ErrorUriTooLong,
};

class HttpRequest {
 public:
  explicit HttpRequest(String* databuffer) noexcept;

  [[nodiscard]] HttpRequestParsingStatus Parse() noexcept;

  [[nodiscard]] HttpVersion GetVersion() const noexcept { return version_; }
  [[nodiscard]] HttpMethod GetMethod() const noexcept { return method_; }
  [[nodiscard]] StringSlice GetTarget() const noexcept { return uri_buffer_.SubSlice(); }
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
  String parse_buffer_{kDefaultBufferCapacity};
  String misc_buffer_{kDefaultBufferCapacity};
  Scanner scanner_{data_};

  HttpVersion version_;
  HttpMethod method_;
  //  HttpUri uri_buffer_{}; TODO: Use this
  String uri_buffer_{kDefaultBufferCapacity};
  HttpEncoding accept_encoding_{None};
  HttpConnection connection_{HttpConnection::Close};
  bool upgrade_insecure_requests_{false};
  String host_{kDefaultHeaderValueCapacity};
  String referrer_{kDefaultHeaderValueCapacity};
  String user_agent_{kDefaultHeaderValueCapacity};
};

}  // namespace cell::http

#endif  // CELL_HTTPREQUEST_HPP
