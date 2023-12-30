// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_REQUEST_HPP
#define CELL_REQUEST_HPP

#include "cell/core/scanner.hpp"
#include "cell/core/string.hpp"
#include "cell/core/string_slice.hpp"
#include "cell/core/weak_string_cache.hpp"
#include "connection.hpp"
#include "encoding.hpp"
#include "method.hpp"
#include "uri.hpp"
#include "version.hpp"

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
  ErrorUriInvalid,
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

  [[nodiscard]] RequestParserResult parse() noexcept;

  [[nodiscard]] Version get_version() const noexcept { return m_version; }
  [[nodiscard]] Method get_method() const noexcept { return m_method; }
  [[nodiscard]] StringSlice get_target() const noexcept { return m_uri.get_data_buffer_as_slice(); }
  [[nodiscard]] StringSlice get_user_agent() const noexcept { return m_user_agent.slice(); }
  [[nodiscard]] StringSlice get_host() const noexcept { return m_host.slice(); }
  [[nodiscard]] StringSlice get_referrer() const noexcept { return m_referrer.slice(); }
  [[nodiscard]] StringSlice get_body() const noexcept { return m_body.slice(); }
  [[nodiscard]] encoding::EncodingSet get_accept_encoding() const noexcept { return m_accept_encoding; }
  [[nodiscard]] Connection get_connection_type() const noexcept { return m_connection; }
  [[nodiscard]] bool get_can_upgrade_insecure_connections() const noexcept {
    return m_upgrade_insecure_requests;
  }

 private:
  static constexpr uint64_t DEFAULT_BUFFER_CAPACITY = 4096;
  static constexpr uint64_t DEFAULT_HEADER_BUFFER_CAPACITY = 4096;

  String* m_data;
  String m_buf1{DEFAULT_BUFFER_CAPACITY};
  String m_buf2{DEFAULT_BUFFER_CAPACITY};
  String m_body{DEFAULT_BUFFER_CAPACITY};

  RequestParserState m_parser_state{RequestParserState::NeedMethod};
  Version m_version{Version::UnsupportedVersion};
  Method m_method{Method::UnsupportedMethod};
  Uri m_uri{};
  encoding::EncodingSet m_accept_encoding{encoding::kNone};
  Connection m_connection{Connection::Close};
  bool m_upgrade_insecure_requests{false};
  String m_host{DEFAULT_HEADER_BUFFER_CAPACITY};
  String m_referrer{DEFAULT_HEADER_BUFFER_CAPACITY};
  String m_user_agent{DEFAULT_HEADER_BUFFER_CAPACITY};
  WeakStringCache m_headers{};
};

}  // namespace cell::http

#endif  // CELL_REQUEST_HPP
