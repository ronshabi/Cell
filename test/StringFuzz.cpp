// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include <cstdint>
#include <cstdlib>

#include "cell/core/String.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  uint8_t buf[Size + 1];
  memset(buf, 0, Size);
  memcpy(buf, Data, Size);
  buf[Size] = 0;

  cell::String mine;
  mine.AppendCString(reinterpret_cast<const char *>(buf));
  return 0;
}


