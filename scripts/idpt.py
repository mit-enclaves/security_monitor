#!/usr/bin/env python3

import struct
import sys

# This scripts constructs a set of identity page tables with all but the MEGA_PAGE_START - MEGA_PAGE_END mapped as giga pages.
# These pages are mega pages via a second level page table (PT_1).
# These page tables are written to reside before address 0x100000000

leaf_permissions = 0b11101111 # D A G (not U) X W R V
node_permissions = 0b00000001 # Node
size_pte = 8

PGSHIFT = 12
PTE_PPN_SHIFT = 10

GIGA_PAGE_SIZE = 0x40000000
MEGA_PAGE_SIZE = 0x200000
KILO_PAGE_SIZE = 0x1000

NUMBER_GIGA_PAGES = 512

MEGA_PAGE_START = 0x080000000
MEGA_PAGE_END   = 0x100000000
NUMBER_MEGA_PAGES = (MEGA_PAGE_END - MEGA_PAGE_START) / MEGA_PAGE_SIZE


SIZE_PAGE_TABLE = (NUMBER_MEGA_PAGES + NUMBER_GIGA_PAGES) * size_pte

END_PAGE_TABLE_ADDRESS = 0x100000000
START_PAGE_TABLE_ADDRESS = END_PAGE_TABLE_ADDRESS - SIZE_PAGE_TABLE

def print_usage():
    print "Usage: %s <output file path>" % (sys.argv[0])

if (len(sys.argv) != 2):
    print_usage()
    sys.exit(1)

name_file = sys.argv[1]

with open(name_file, 'wb') as f:

    next_page = START_PAGE_TABLE_ADDRESS
    next_page += NUMBER_GIGA_PAGES

    # Generate the giga page table (PT_0)
    for i in range(NUMBER_GIGA_PAGES):
        # if i corresponds to a giga page in the mega page range, it is a node in this IDPT.
        if ((i * GIGA_PAGE_SIZE) >= MEGA_PAGE_START) and ((i * GIGA_PAGE_SIZE) < MEGA_PAGE_END):
            pte = ((next_page >> PGSHIFT) << PTE_PPN_SHIFT) | node_permissions
            next_page += KILO_PAGE_SIZE
        else:
            # to get a real identity mapping, the PPN[2] field should be filled with 1's in the upper 17 bits,
            # but we don't really care about those bits because either way these addresses are not legal in
            # our system.
            # i corresponds to bits 38 down to 30 of the virtual address
            # if bit 38 is set to 1, then bits 63 down to 39 must be set to 1 as well
            pte = ((i * GIGA_PAGE_SIZE >> PGSHIFT) << PTE_PPN_SHIFT) | leaf_permissions # point to PT_1
        # write pte to f
        bytes_to_write = struct.pack('<Q', pte)
        # if this assert fails, you need to find a different way to pack the int pte into 8 bytes in little-endian order
        assert( len(bytes_to_write) == size_pte)
        f.write(bytes_to_write)


    # Generate the mega page table (PT_0) for pages starting at 0x80000000
    for i in range(NUMBER_MEGA_PAGES):
        pte = (((MEGA_PAGE_START+(i * MEGA_PAGE_SIZE)) >> PGSHIFT) << PTE_PPN_SHIFT) | leaf_permissions
        # write pte to f
        bytes_to_write = struct.pack('<Q', pte)
        # if this assert fails, you need to find a different way to pack the int pte into 8 bytes in little-endian order
        assert( len(bytes_to_write) == size_pte )
        f.write(bytes_to_write)
