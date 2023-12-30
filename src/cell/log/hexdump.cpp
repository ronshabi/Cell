// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include "cell/core/types.hpp"
#include <cstdio>

namespace cell::log {

// Debug only function
// If 'LOGGER_DEBUG' is not set, this is a nop
void hexdump(const u8* ptr, int length, int line_width) {
#ifdef LOGGER_DEBUG
  printf("--- cell::hexdump [%p][:%d] --- \n", ptr, length);

  for (int i = 0; i < length; ++i) {
    if (i % line_width == 0) {
      printf("\n");
    }
    printf("%02x ", ptr[i]);
  }
}
#endif
}