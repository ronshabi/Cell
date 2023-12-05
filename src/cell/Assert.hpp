#ifndef CELL_ASSERT_HPP
#define CELL_ASSERT_HPP

#include <cstdio>

#ifdef DEBUG

#define CELL_ASSERT(expr)                                                        \
  {                                                                              \
    if (!(expr)) [[unlikely]] {                                                  \
      printf("[CELL_ASSERT] Assertion '%s' failed\n", #expr);                    \
      printf("              In: %s, Line: %d\n", __PRETTY_FUNCTION__, __LINE__); \
      __builtin_unreachable();                                                   \
    }                                                                            \
  }
#else
#define CELL_ASSERT(expr)
#endif

#endif  // CELL_ASSERT_HPP
