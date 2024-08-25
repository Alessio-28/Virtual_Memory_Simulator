#pragma once

#include "page_table.h"
#include "linked_list.h"

typedef struct PageListItem{
    ListItem* prev;
    ListItem* next;
    Page* item;
} PageListItem;

typedef struct MMU{
    PTBR pt;
    PTLR pt_len;
    // Circular list pointer    
} MMU;


void MMU_writeByte(MMU* mmu, uint32_t pos, char c);
char* MMU_readByte(MMU* mmu, uint32_t pos);
void MMU_exception(MMU* mmu, uint32_t pos);