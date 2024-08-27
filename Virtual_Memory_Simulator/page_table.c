#pragma once

#include "page_table.h"
#include <assert.h>

void PageTable_init(PageTable* pt, uint32_t* pt_len, char* phys_mem){
    assert(phys_mem && "Physical memory poiter must not be NULL");
    pt = (PageTable*) phys_mem;

    for(uint32_t i = 0; i < PAGES; ++i){
        pt->table[i].frame_index = -1;
        pt->table[i].unswappable = 0;
        pt->table[i].valid = 0;
        pt->table[i].read = 0;
        pt->table[i].write = 0;
    }

    *pt_len = sizeof(PageTable);    
}

uint32_t getPageIndex(uint32_t virt_addr){ return virt_addr >> PAGE_OFFSET_SIZE; }

uint32_t getFrameIndex(uint32_t phys_addr){ return phys_addr >> PAGE_OFFSET_SIZE; }

uint32_t getOffset(uint32_t addr){
    uint32_t mask = (1 << PAGE_OFFSET_SIZE) - 1;
    return addr & mask;
}

uint32_t getPhysicalAddress(PageTable *pt, uint32_t virt_addr)
{
    uint32_t page_offset = getOffset(virt_addr);
    uint32_t page_index = getPageIndex(virt_addr);

    uint32_t frame_index = pt->table[page_index].frame_index;
    uint32_t phys_addr = (frame_index << PAGE_OFFSET_SIZE) | page_offset;

    return phys_addr;
}

AddressingResult AddressIsValid(PageTable* pt, uint32_t virt_addr){
    uint32_t page_index = getPageIndex(virt_addr);

    if(pt->table[page_index].unswappable) return Invalid;
    else if(pt->table[page_index].valid) return Valid;
    else return PageNotAllocated;
}
