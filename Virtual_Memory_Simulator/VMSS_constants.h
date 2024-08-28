#pragma once

#include <stdint.h>

#define VIRT_MEM_SIZE_BIT 24    // Default values
#define PHYS_MEM_SIZE_BIT 20    // Default values
#define PAGE_SIZE_BIT     12    // Default values

extern uint32_t virt_mem_size_bit;
extern uint32_t phys_mem_size_bit;
extern uint32_t page_size_bit;

#define VIRT_MEM_SIZE ((uint32_t)(1 << virt_mem_size_bit))
#define PHYS_MEM_SIZE ((uint32_t)(1 << phys_mem_size_bit))
#define PAGE_SIZE     ((uint32_t)(1 << page_size_bit))

#define PAGE_INDEX_SIZE  (virt_mem_size_bit - page_size_bit)
#define FRAME_INDEX_SIZE (phys_mem_size_bit - page_size_bit)

#define PAGES  ((uint32_t)(1 << PAGE_INDEX_SIZE))
#define FRAMES ((uint32_t)(1 << FRAME_INDEX_SIZE))

#define PAGE_OFFSET_SIZE      page_size_bit
#define LOGICAL_ADDRESS_SIZE  virt_mem_size_bit
#define PHYSICAL_ADDRESS_SIZE phys_mem_size_bit

#define SWAP_FILE "swap.txt"
