// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_URI_HPP
#define CELL_URI_HPP

#include <cstdint>
#include <functional>
#include <unordered_map>

#include "cell/core/string.hpp"
#include "cell/core/string_slice.hpp"
#include "cell/core/weak_string_cache.hpp"

namespace cell::http {

enum class UriParserResult {
  Ok,
  UnsupportedUriType,
  DecodingQueryValueFailed,
  DecodingPathFailed,
};

enum class UriParserState { GetType, GetPath, GetQueryKey, GetQueryValue };

enum class UriType {
  Absolute,
  Relative,
};

class Uri {
 public:
  explicit Uri() noexcept = default;

  [[nodiscard]] StringSlice get_path_raw() const noexcept { return m_path.slice(); }
  [[nodiscard]] StringSlice get_path_decoded() const noexcept { return m_path_decoded.slice(); }
  [[nodiscard]] const WeakStringCache & get_queries() const noexcept {
    return m_queries;
  }

  [[nodiscard]] UriParserResult parse() noexcept;
  [[nodiscard]] StringSlice get_data_buffer_as_slice() const noexcept { return m_data.slice(); }
  [[nodiscard]] static bool decode(StringSlice slice, String& out);

  void append_byte_to_data_buffer(uint8_t ch) noexcept { m_data.append_byte(ch); }
  void clear_data_buffer() noexcept { m_data.clear(); }
  void set_data_buffer(StringSlice slice) noexcept { m_data = String(slice);
  }

 private:
  static constexpr uint64_t DEFAULT_URI_BUFFER_CAPACITY = 1024;
  static constexpr uint64_t DEFAULT_URI_PATH_BUFFER_CAPACITY = 256;
  static constexpr uint64_t DEFAULT_QUERY_KEY_BUFFER_CAPACITY = 64;
  static constexpr uint64_t DEFAULT_QUERY_VALUE_BUFFER_CAPACITY = 256;

  UriParserState state_{UriParserState::GetType};
  String m_data{DEFAULT_URI_BUFFER_CAPACITY};
  String m_path{DEFAULT_URI_PATH_BUFFER_CAPACITY};
  String m_path_decoded{DEFAULT_URI_PATH_BUFFER_CAPACITY};
  String m_query_key{DEFAULT_QUERY_KEY_BUFFER_CAPACITY};
  String m_query_value{DEFAULT_QUERY_VALUE_BUFFER_CAPACITY};
  String m_query_value_decoded{DEFAULT_QUERY_VALUE_BUFFER_CAPACITY};
  WeakStringCache m_queries{};
  UriType m_uri_type{UriType::Absolute};
};

}  // namespace cell::http

#endif  // CELL_URI_HPP
