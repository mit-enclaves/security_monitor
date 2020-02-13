#ifndef PLATFORM_ASSERT_H
#define PLATFORM_ASSERT_H

#include <kernel/kernel_api.h>
#include <platform.h>

#define assert(x) ({ if (!(x)) die("assertion failed: %s", #x); })
#define die(str, ...) ({ printm("%s:%d: " str "\n", __FILE__, __LINE__, ##__VA_ARGS__); platform_panic(); })

#endif // PLATFORM_ASSERT_H
