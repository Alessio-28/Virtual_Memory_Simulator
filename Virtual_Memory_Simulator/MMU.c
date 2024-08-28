#include "MMU.h"
#include "swap.h"
#include <stdio.h>
#include <assert.h>

void MMU_init(MMU* mmu, char* phys_mem){
    assert(mmu && "MMU poiter must not be NULL");
    assert(phys_mem && "Physical memory poiter must not be NULL");

    mmu->mem_ptr = phys_mem;
    mmu->cricular_list_index = 0;

    PageTable_init(mmu, phys_mem);

    // Set pages used to allocate the page table and free list
    uint32_t pages_for_data_structures = ((uint32_t)((mmu->pt_len + (FRAMES * sizeof(int))) / PAGE_SIZE)) + 1;
    for(uint32_t i = 0; i < pages_for_data_structures; ++i){
        mmu->pt[i].frame_index = i;
        mmu->pt[i].unswappable = 1;
        mmu->pt[i].valid = 1;
        mmu->pt[i].read = 1;
        mmu->pt[i].write = 1;
    }

    mmu->free_list.list = (int*)(phys_mem + mmu->pt_len);

    ArrayList_init(&(mmu->free_list), pages_for_data_structures, FRAMES, FRAMES);

    Swap_init(SWAP_FILE);
}

void PageTable_init(MMU* mmu, char* phys_mem){
    assert(phys_mem && "Physical memory poiter must not be NULL");
    mmu->pt = (PageTable)phys_mem;

    for(uint32_t i = 0; i < PAGES; ++i){
        mmu->pt[i].frame_index = 0;
        mmu->pt[i].unswappable = 0;
        mmu->pt[i].valid = 0;
        mmu->pt[i].read = 0;
        mmu->pt[i].write = 0;
    }

    mmu->pt_len = sizeof(Page)*PAGES;
}


void MMU_writeByte(MMU* mmu, uint32_t virt_addr, char c){
    AddressingResult res = AddressIsValid(mmu, virt_addr);

    if(res == Invalid){
        printf("Address not valid\n");
        return;
    }

    if(res == PageNotInMemory)
        MMU_exception(mmu, virt_addr);

    uint32_t page_index = getPageIndex(virt_addr);
    mmu->pt[page_index].write = 1;
    uint32_t phys_addr = getPhysicalAddress(mmu, virt_addr);

    mmu->mem_ptr[phys_addr] = c;
}

char* MMU_readByte(MMU* mmu, uint32_t virt_addr){
    AddressingResult res = AddressIsValid(mmu, virt_addr);

    if(res == Invalid){
        printf("Address not valid\n");
        return NULL;
    }

    if(res == PageNotInMemory)
        MMU_exception(mmu, virt_addr);

    uint32_t page_index = getPageIndex(virt_addr);
    mmu->pt[page_index].read = 1;
    uint32_t phys_addr = getPhysicalAddress(mmu, virt_addr);

    return &(mmu->mem_ptr[phys_addr]);
}

void MMU_exception(MMU* mmu, uint32_t virt_addr){
    uint32_t page_index_in = getPageIndex(virt_addr);
    int free_frame = getElement(&(mmu->free_list));

    if(free_frame == -1){
        uint32_t page_index_out = SecondChance(mmu);
        uint32_t frame_index = mmu->pt[page_index_out].frame_index;

        Swap_out_and_in(SWAP_FILE, mmu->mem_ptr, frame_index, page_index_in, page_index_out);
        addPage(mmu, page_index_in, frame_index);
    }
    else{
        Swap_in(SWAP_FILE, mmu->mem_ptr, (uint32_t)free_frame, page_index_in);
        addPage(mmu, page_index_in, free_frame);
    }
}

uint32_t SecondChance(MMU* mmu){
    PageTable pt = mmu->pt;

    while(1){
        // Return first page found that is not read and not written
        for(uint32_t i = mmu->cricular_list_index; i < PAGES; ++i){
            if(pt[i].valid && !(pt[i].unswappable) && !(pt[i].read) && !(pt[i].write)){
                // Update circular list index to the next element int the list
                mmu->cricular_list_index = i+1 == PAGES ? 0 : i+1;
                return i;
            }
        }

        // Return first page found that is not read but written
        // Set read bit to 0
        for(uint32_t i = 0; i < PAGES; ++i){
            if(pt[i].valid && !(pt[i].unswappable)){
                if(!(pt[i].read) && pt[i].write){
                    // Update circular list index to the next element int the list
                    mmu->cricular_list_index = i+1 == PAGES ? 0 : i+1;
                    return i;
                }
                pt[i].read = 0;
            }
        }

        mmu->cricular_list_index = 0;
    }
}

void addPage(MMU* mmu, uint32_t page_index, uint32_t frame_index){
    mmu->pt[page_index].frame_index = frame_index;
    mmu->pt[page_index].unswappable = 0;
    mmu->pt[page_index].valid = 1;
    mmu->pt[page_index].read = 0;
    mmu->pt[page_index].write = 0;
}

void deletePage(MMU* mmu, uint32_t page_index){
    mmu->pt[page_index].frame_index = 0;
    mmu->pt[page_index].unswappable = 0;
    mmu->pt[page_index].valid = 0;
    mmu->pt[page_index].read = 0;
    mmu->pt[page_index].write = 0;
}

uint32_t getPageIndex(uint32_t virt_addr){ return virt_addr >> PAGE_OFFSET_SIZE; }

uint32_t getFrameIndex(uint32_t phys_addr){ return phys_addr >> PAGE_OFFSET_SIZE; }

uint32_t getOffset(uint32_t addr){
    uint32_t mask = (1 << PAGE_OFFSET_SIZE) - 1;
    return addr & mask;
}

uint32_t getPhysicalAddress(MMU* mmu, uint32_t virt_addr){
    uint32_t page_offset = getOffset(virt_addr);
    uint32_t page_index = getPageIndex(virt_addr);

    uint32_t frame_index = mmu->pt[page_index].frame_index;
    uint32_t phys_addr = (frame_index << PAGE_OFFSET_SIZE) | page_offset;

    return phys_addr;
}

AddressingResult AddressIsValid(MMU* mmu, uint32_t virt_addr){
    uint32_t page_index = getPageIndex(virt_addr);

    if(mmu->pt[page_index].unswappable) return Invalid;
    else if(mmu->pt[page_index].valid) return Valid;
    else return PageNotInMemory;
}