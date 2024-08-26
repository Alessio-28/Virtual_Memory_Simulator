#pragma once

#include "VMSS_constants.h"
#include "array_list.h"

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

typedef struct MMU{
    char* mem_ptr;
    PageTable* pt;
    uint32_t pt_len;
    
    ArrayList free_list;
} MMU;

void PageTable_init(PageTable* pt, uint32_t* pt_len, char* phys_mem);
void MMU_init(MMU* mmu, char* phys_mem);

uint32_t getPhysicalAddress(PageTable* pt, uint32_t virt_addr);
int isValidAddress();

void MMU_writeByte(MMU* mmu, uint32_t pos, char c);
char* MMU_readByte(MMU* mmu, uint32_t pos);
void MMU_exception(MMU* mmu, uint32_t pos);