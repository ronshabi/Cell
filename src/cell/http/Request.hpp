// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_REQUEST_HPP
#define CELL_REQUEST_HPP

#include "Connection.hpp"
#include "Encoding.hpp"
#include "Method.hpp"
#include "Uri.hpp"
#include "Version.hpp"
#include "cell/core/Scanner.hpp"
#include "cell/core/String.hpp"
#include "cell/core/StringSlice.hpp"
#include "cell/core/WeakStringCache.hpp"

namespace cell::http {

enum class RequestParserResult {
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

enum class RequestParserState {
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


class Request {
 public:
  explicit Request(String* databuffer) noexcept;

  [[nodiscard]] RequestParserResult Parse() noexcept;

  [[nodiscard]] Version GetVersion() const noexcept { return version_; }
  [[nodiscard]] Method GetMethod() const noexcept { return method_; }
  [[nodiscard]] StringSlice GetTarget() const noexcept { return uri_.GetPath(); }
  [[nodiscard]] StringSlice GetUserAgent() const noexcept { return user_agent_.SubSlice(); }
  [[nodiscard]] StringSlice GetHost() const noexcept { return host_.SubSlice(); }
  [[nodiscard]] StringSlice GetReferrer() const noexcept { return referrer_.SubSlice(); }
  [[nodiscard]] encoding::EncodingSet GetAcceptEncoding() const noexcept { return accept_encoding_; }
  [[nodiscard]] Connection GetConnectionType() const noexcept { return connection_; }
  [[nodiscard]] bool CanUpgradeInsecureConnections() const noexcept {
    return upgrade_insecure_requests_;
  }

 private:
  static constexpr uint64_t kDefaultBufferCapacity = 4096;
  static constexpr uint64_t kDefaultHeaderValueCapacity = 4096;

  String* data_;
  String buf1{kDefaultBufferCapacity};
  String buf2{kDefaultBufferCapacity};

  RequestParserState parser_state_{RequestParserState::NeedMethod};

  Version version_{Version::UnsupportedVersion};
  Method method_{Method::UnsupportedMethod};
  Uri uri_{};
  encoding::EncodingSet accept_encoding_{encoding::kNone};
  Connection connection_{Connection::Close};
  bool upgrade_insecure_requests_{false};
  String host_{kDefaultHeaderValueCapacity};
  String referrer_{kDefaultHeaderValueCapacity};
  String user_agent_{kDefaultHeaderValueCapacity};
  WeakStringCache headers_{};
};

}  // namespace cell::http

#endif  // CELL_REQUEST_HPP
