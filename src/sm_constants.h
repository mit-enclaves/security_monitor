#ifndef SECURITY_MONITOR_CONSTANTS_H
#define SECURITY_MONITOR_CONSTANTS_H

#include "parameters.h"

// Derived parameters
// ------------------

#define PAGE_SIZE (1<<PAGE_SHIFT)
#define PAGE_MASK (PAGE_SIZE-1)

#define REGION_SIZE (1<<REGION_SHIFT)
#define REGION_MASK (REGION_SIZE-1)
#define NUM_REGIONS (RAM_SIZE / REGION_SIZE)
#define NUM_REGION_PAGES (1<<(REGION_SHIFT-PAGE_SHIFT))

// Validate parameterization
// -------------------------

// TODO: ENUMs fit in their respective bit fields

// TODO: region_type_t fits into uint8_t

#if (NUM_CORES != 1)
  #error One core is currently supported! While the SM appropriately includes locks, the initialization code does not currently tolerate multiple cores executing the init code.
#endif

#if (NUM_REGIONS > 64)
  #error The platform uses an XLEN (uint64_t) register to set a region permission bitmap; The platform code therefore can only work with <= 64 regions.
#endif

#if (REGION_SHIFT <= PAGE_SHIFT)
  #error Regions must be larger than a page in size. In practice, they should be far larger in order to be useful, as the leading pages of a metadata region are reserved for a metadata page map structure.
#endif

#endif // SECURITY_MONITOR_CONSTANTS_H
