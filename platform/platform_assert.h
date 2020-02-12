#ifndef PLATFORM_ASSERT_H
#define PLATFORM_ASSERT_H

#include <platform.h>
#include "htif/htif.h"

#define assert(x) ({ if (!(x)) die(conca("assertion failed: ", #x)); })

#define conca(x, y) (x##y)

#define die(str) ({ \
  print_str(#str); \
  platform_panic(); })

#endif // PLATFORM_ASSERT_H
