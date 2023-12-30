// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_LOG_HPP
#define CELL_LOG_HPP

#include "cell/core/types.hpp"

#ifdef LOGGER_DEBUG
#define CELL_LOG_DEBUG_SIMPLE(fmt)                                \
  {                                                               \
    printf("[DEBUG] [%s -- %d] ", __PRETTY_FUNCTION__, __LINE__); \
    printf(fmt);                                                  \
    printf("\n");                                                 \
  }

#define CELL_LOG_DEBUG(fmt, ...)                                  \
  {                                                               \
    printf("[DEBUG] [%s -- %d] ", __PRETTY_FUNCTION__, __LINE__); \
    printf(fmt, __VA_ARGS__);                                     \
    printf("\n");                                                 \
  }

#else
#define CELL_LOG_DEBUG_SIMPLE(...)
#define CELL_LOG_DEBUG(...)
#endif


namespace cell::log {

// Debug only function
// If 'LOGGER_DEBUG' is not set, this is a nop
void hexdump(const u8* ptr, int length, int line_width = 16);
}

#endif  // CELL_LOG_HPP
