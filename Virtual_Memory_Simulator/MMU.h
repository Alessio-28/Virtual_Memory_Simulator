#pragma once

#include "page_table.h"
#include "array_list.h"

typedef struct MMU{
    char* mem_ptr;
    PageTable* pt;
    uint32_t pt_len;
    
    ArrayList free_list;
} MMU;

void MMU_init(MMU* mmu, char* phys_mem);

int isValidAddress();

void MMU_writeByte(MMU* mmu, uint32_t virt_addr, char c);
char* MMU_readByte(MMU* mmu, uint32_t virt_addr);
void MMU_exception(MMU* mmu, uint32_t virt_addr);