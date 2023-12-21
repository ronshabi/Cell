// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_ENCODING_HPP
#define CELL_ENCODING_HPP

#include <cstdint>

#include "cell/core/StringSlice.hpp"

namespace cell::http {



namespace encoding {
using EncodingSet = uint64_t;

constexpr EncodingSet kErrorParsing = static_cast<EncodingSet>(-1);
constexpr EncodingSet kNone = 0;
constexpr EncodingSet kDeflate = 0b1;
constexpr EncodingSet kGzip = 0b10;
constexpr EncodingSet kBrotli = 0b100;
constexpr EncodingSet kZstd = 0b100;

[[nodiscard]] EncodingSet ParseFromString(StringSlice slice) noexcept;

}

}

#endif  // CELL_ENCODING_HPP
