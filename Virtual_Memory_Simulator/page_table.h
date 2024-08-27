#pragma once

#include "VMSS_constants.h"

typedef enum AddressingResult{
    Valid = 0,
    Invalid = -1,
    PageNotAllocated = -2
} AddressingResult;

typedef struct Page{
    uint32_t frame_index: FRAME_INDEX_SIZE;
    uint8_t unswappable: 1;
    uint8_t valid: 1;
    uint8_t read: 1;
    uint8_t write: 1;
} Page;

typedef struct PageTable{
    Page table[PAGES];
} PageTable;

void PageTable_init(PageTable* pt, uint32_t* pt_len, char* phys_mem);

// Get page index from virtual address
uint32_t getPageIndex(uint32_t virt_addr);

// Get frame index from physical address
uint32_t getFrameIndex(uint32_t phys_addr);

// Get page offset from either virtual or physical address
uint32_t getOffset(uint32_t addr);

// Translate virtual address to physical address.
// Assume virt_addr is a valid address
uint32_t getPhysicalAddress(PageTable* pt, uint32_t virt_addr);

AddressingResult AddressIsValid(PageTable* pt, uint32_t virt_addr);