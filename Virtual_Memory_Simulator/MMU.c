#include "MMU.h"
#include "assert.h"

void PageTable_init(PageTable* pt, uint32_t* pt_len, char* phys_mem){
    assert(pt && "Page table poiter must not be NULL");
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
}

uint32_t getPhysicalAddress(PageTable* pt, uint32_t virt_addr){
    uint32_t offset_mask = (1 << PAGE_OFFSET_SIZE) - 1;
    uint32_t page_offset = virt_addr & offset_mask;
    uint32_t page_index = (virt_addr >> PAGE_OFFSET_SIZE);

    uint32_t frame_index = pt->table[page_index].frame_index;
    uint32_t phys_addr = (frame_index << PAGE_OFFSET_SIZE) | page_offset;

    return phys_addr;
}

void MMU_writeByte(MMU* mmu, uint32_t pos, char c){
    
}
char* MMU_readByte(MMU* mmu, uint32_t pos);
void MMU_exception(MMU* mmu, uint32_t pos);