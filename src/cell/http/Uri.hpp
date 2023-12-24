// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_URI_HPP
#define CELL_URI_HPP

#include <cstdint>
#include <functional>
#include <unordered_map>

#include "cell/core/String.hpp"
#include "cell/core/WeakStringCache.hpp"

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

  [[nodiscard]] StringSlice GetPath() const noexcept { return path_.SubSlice(); }

  void AppendByteToDataBuffer(uint8_t ch) noexcept { data_.AppendByte(ch); }

  void ClearDataBuffer() noexcept { data_.Clear(); }

  [[nodiscard]] UriParserResult Parse() noexcept;

  [[nodiscard]] StringSlice GetDataBufferAsSlice() const noexcept { return data_.SubSlice(); }

  [[nodiscard]] static bool Decode(StringSlice slice, String& out);

 private:
  static constexpr uint64_t kDefaultUriBufferCapacity = 1024;
  static constexpr uint64_t kDefaultUriPathCapacity = 256;
  static constexpr uint64_t kDefaultQueryKeyBufferCapacity = 64;
  static constexpr uint64_t kDefaultQueryValueBufferCapacity = 256;

  UriParserState state_{UriParserState::GetType};

  String data_{kDefaultUriBufferCapacity};

  String path_{kDefaultUriPathCapacity};
  String path_decoded_{kDefaultUriPathCapacity};
  String query_key_{kDefaultQueryKeyBufferCapacity};
  String query_value_{kDefaultQueryValueBufferCapacity};
  String query_value_decoded_{kDefaultQueryValueBufferCapacity};

  WeakStringCache queries_{};

  UriType uri_type_{UriType::Absolute};
};

}  // namespace cell::http

#endif  // CELL_URI_HPP
