// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_ENCODING_HPP
#define CELL_ENCODING_HPP

#include <cstdint>

#include "cell/core/string_slice.hpp"

namespace cell::http::encoding {
using EncodingSet = uint64_t;
constexpr uint64_t ENCODING_BUFFER_SIZE = 16;

constexpr EncodingSet ERROR_PARSING = static_cast<EncodingSet>(-1);
constexpr EncodingSet kNone = 0;
constexpr EncodingSet DEFLATE = 0b1;
constexpr EncodingSet GZIP = 0b10;
constexpr EncodingSet BROTLI = 0b100;
constexpr EncodingSet ZSTD = 0b100;

[[nodiscard]] EncodingSet parse_from_request_header(const StringSlice slice) noexcept;

}

#endif  // CELL_ENCODING_HPP
