#pragma once

#include <stdint.h>

#define VIRT_MEM_SIZE_BIT 24
#define PHYS_MEM_SIZE_BIT 20
#define PAGE_SIZE_BIT     12

#define VIRT_MEM_SIZE ((uint32_t)(1 << VIRT_MEM_SIZE_BIT))
#define PHYS_MEM_SIZE ((uint32_t)(1 << PHYS_MEM_SIZE_BIT))
#define PAGE_SIZE     ((uint32_t)(1 << PAGE_SIZE_BIT))

#define PAGE_INDEX_SIZE  (VIRT_MEM_SIZE_BIT - PAGE_SIZE_BIT)
#define FRAME_INDEX_SIZE (PHYS_MEM_SIZE_BIT - PAGE_SIZE_BIT)

#define PAGES  ((uint32_t)(1 << PAGE_INDEX_SIZE))
#define FRAMES ((uint32_t)(1 << FRAME_INDEX_SIZE))

#define PAGE_OFFSET_SIZE      PAGE_SIZE_BIT
#define LOGICAL_ADDRESS_SIZE  VIRT_MEM_SIZE_BIT
#define PHYSICAL_ADDRESS_SIZE PHYS_MEM_SIZE_BIT

#define SWAP_FILE "swap.txt"