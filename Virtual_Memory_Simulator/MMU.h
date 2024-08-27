#pragma once

#include "VMSS_constants.h"
#include "array_list.h"

typedef enum AddressingResult{
    Valid = 0,
    Invalid = -1,
    PageNotInMemory = -2
} AddressingResult;

typedef struct Page{
    uint32_t frame_index: FRAME_INDEX_SIZE;
    uint8_t unswappable: 1;
    uint8_t valid: 1;
    uint8_t read: 1;
    uint8_t write: 1;
} Page;

typedef Page* PageTable;

typedef struct MMU{
    // Pointer to the physical memory
    char* mem_ptr;
    // Pointer to Page table, stored in main memory
    PageTable pt;
    // Length of the page table
    uint32_t pt_len;
    
    // List of free frames
    // list and start elements are stored in MMU
    // Array pointed by list is stored in main memory, below the page table
    ArrayList free_list;
} MMU;


void MMU_init(MMU* mmu, char* phys_mem);
void PageTable_init(MMU* mmu, char* phys_mem);

void MMU_writeByte(MMU* mmu, uint32_t virt_addr, char c);

// If virt_addr is not valid, return NULL
char* MMU_readByte(MMU* mmu, uint32_t virt_addr);

void MMU_exception(MMU* mmu, uint32_t virt_addr);

// Get page index from virtual address
uint32_t getPageIndex(uint32_t virt_addr);

// Get frame index from physical address
uint32_t getFrameIndex(uint32_t phys_addr);

// Get page offset from either virtual or physical address
uint32_t getOffset(uint32_t addr);

// Translate virtual address to physical address.
// Assume virt_addr is a valid address
uint32_t getPhysicalAddress(MMU* mmu, uint32_t virt_addr);

AddressingResult AddressIsValid(MMU* mmu, uint32_t virt_addr);