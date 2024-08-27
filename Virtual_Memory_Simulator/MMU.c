#pragma once

#include "MMU.h"
#include "swap.h"
#include <stdio.h>
#include <assert.h>

void MMU_init(MMU* mmu, char* phys_mem){
    assert(mmu && "MMU poiter must not be NULL");
    assert(phys_mem && "Physical memory poiter must not be NULL");

    mmu->mem_ptr = phys_mem;

    PageTable_init(mmu->pt, &(mmu->pt_len), phys_mem);

    uint32_t pages_for_data_structures = ((uint32_t)((mmu->pt_len + (FRAMES * sizeof(int))) / PAGE_SIZE)) + 1;

    for(uint32_t i = 0; i < pages_for_data_structures; ++i){
        mmu->pt->table[i].frame_index = i;
        mmu->pt->table[i].unswappable = 1;
        mmu->pt->table[i].valid = 1;
        mmu->pt->table[i].read = 1;
        mmu->pt->table[i].write = 1;
    }

    mmu->free_list.list = (int*)(phys_mem + mmu->pt_len);

    ArrayList_init(&(mmu->free_list), pages_for_data_structures, FRAMES, FRAMES);

    Swap_init(SWAP_FILE);
}

void MMU_writeByte(MMU* mmu, uint32_t virt_addr, char c){
    
}

char* MMU_readByte(MMU* mmu, uint32_t virt_addr){
    AddressingResult res = AddressIsValid(mmu->pt, virt_addr);
    if(res == Valid){
        uint32_t page_index = getPageIndex(virt_addr);
        mmu->pt->table[page_index].read = 1;
        uint32_t phys_addr = getPhysicalAddress(mmu->pt, virt_addr);
        return &(mmu->mem_ptr[phys_addr]);
    }
    else if(res == Invalid){
        printf("Address not valid\n");
        return NULL;
    }
}
void MMU_exception(MMU* mmu, uint32_t virt_addr);