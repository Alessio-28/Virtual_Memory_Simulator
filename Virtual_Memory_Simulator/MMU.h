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
    uint32_t cricular_list_index;
    // Length of the page table
    uint32_t pt_len;
    
    // List of free frames
    // list and start elements are stored in MMU
    // Array pointed by list is stored in main memory, after the page table
    ArrayList free_list;
} MMU;

// Make MMU_writeByte and PrintPageInMemory return int

void MMU_init(MMU* mmu, char* phys_mem);
void PageTable_init(MMU* mmu, char* phys_mem);

int MMU_writeByte(MMU* mmu, const uint32_t virt_addr, const char c);

// If virt_addr is not valid, return NULL
const char* MMU_readByte(MMU* mmu, const uint32_t virt_addr);

void MMU_exception(MMU* mmu, const uint32_t virt_addr);

// Choose the page to be overwritten
// Set page_index to the index of the chosen page
// Return the class of the page
// Return 0 if the page has not been read and not been written
// Return 1 if the page has not been read but has been written
uint8_t SecondChance(MMU* mmu, uint32_t* page_index);

void addPage(MMU* mmu, const uint32_t page_index, const uint32_t frame_index);

void removePage(MMU* mmu, const uint32_t page_index);

// Translate virtual address to physical address.
// Assume virt_addr is a valid address
uint32_t getPhysicalAddress(const MMU* mmu, const uint32_t virt_addr);

// Get page index from virtual address
uint32_t getPageIndex(const uint32_t virt_addr);

// Get frame index from physical address
uint32_t getFrameIndex(const uint32_t phys_addr);

// Get page offset from either virtual or physical address
uint32_t getOffset(const uint32_t addr);

AddressingResult AddressIsValid(const MMU* mmu, const uint32_t virt_addr);



void PrintMMU(const MMU mmu);
void PrintPageTable(const MMU mmu);
// Print data in physical memory and info of pages stored in it
void PrintPhysicalMemory(const MMU mmu);
// Print pages in the working set and their info
void PrintWorkingSet(const MMU mmu);
void PrintPageInMemory(const MMU mmu, const uint32_t page_index);