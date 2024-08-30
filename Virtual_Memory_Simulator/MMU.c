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
    
    mmu->free_list.list = (int*)(phys_mem + mmu->pt_len);
    ArrayList_init(&(mmu->free_list), FRAMES);


    // Set pages used to allocate the page table and free list
    uint32_t pages_for_data_structures = 1;
    uint32_t temp = (mmu->pt_len + (mmu->free_list.max_size * sizeof(int)));
    while((pages_for_data_structures * PAGE_SIZE) < temp)
        pages_for_data_structures++;
    
    assert((pages_for_data_structures <= FRAMES) && "Not enough memory to store MMU data structures");

    for(uint32_t i = 0; i < pages_for_data_structures; ++i){
        mmu->pt[i].frame_index = getElement(&(mmu->free_list));
        mmu->pt[i].unswappable = 1;
        mmu->pt[i].valid = 1;
        mmu->pt[i].read = 1;
        mmu->pt[i].write = 1;
    }
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
    assert((virt_addr >= 0 || virt_addr < VIRT_MEM_SIZE) && "Virtual address must be non negative and less than VIRT_MEM_SIZE");
    AddressingResult res = AddressIsValid(mmu, virt_addr);

    if(res == Invalid){
        printf("Invalid Address\n");
        return;
    }

    if(res == PageNotInMemory)
        MMU_exception(mmu, virt_addr);

    uint32_t page_index = getPageIndex(virt_addr);
    mmu->pt[page_index].read  = 1;
    mmu->pt[page_index].write = 1;
    uint32_t phys_addr = getPhysicalAddress(mmu, virt_addr);

    mmu->mem_ptr[phys_addr] = c;

#ifdef _TEST_
    printf("Character written on page %u on frame %u\n", page_index, getFrameIndex(phys_addr));
#endif
}

char* MMU_readByte(MMU* mmu, uint32_t virt_addr){
    assert((virt_addr >= 0 || virt_addr < VIRT_MEM_SIZE) && "Virtual address must be non negative and less than VIRT_MEM_SIZE");
    AddressingResult res = AddressIsValid(mmu, virt_addr);

    if(res == Invalid){
        printf("Invalid Address\n\n");
        return NULL;
    }

    if(res == PageNotInMemory)
        MMU_exception(mmu, virt_addr);

    uint32_t page_index = getPageIndex(virt_addr);
    mmu->pt[page_index].read = 1;
    uint32_t phys_addr = getPhysicalAddress(mmu, virt_addr);

#ifdef _TEST_
    printf("Character read from page %u on frame %u\n", page_index, getFrameIndex(phys_addr));
#endif

    return &(mmu->mem_ptr[phys_addr]);
}

void MMU_exception(MMU* mmu, uint32_t virt_addr){
    uint32_t page_index_in = getPageIndex(virt_addr);
    int free_frame = getElement(&(mmu->free_list));

    if(free_frame == -1){
        uint32_t page_index_out;
        uint8_t class = SecondChance(mmu, &page_index_out);
        uint32_t frame_index = mmu->pt[page_index_out].frame_index;
        // Swap_out_and_in(SWAP_FILE, mmu->mem_ptr, frame_index, page_index_out, page_index_in);

        if(class == 1){
            Swap_out(SWAP_FILE, mmu->mem_ptr, frame_index, page_index_out);
#ifdef _TEST_
            printf("Page %u swaped out from frame %u\n", page_index_out, frame_index);
#endif
        }

        Swap_in(SWAP_FILE, mmu->mem_ptr, frame_index, page_index_in);
        removePage(mmu, page_index_out);
        addPage(mmu, page_index_in, frame_index);
#ifdef _TEST_
        printf("Page %u swaped in on frame %u\n", page_index_in, frame_index);
#endif
    }
    else{
        Swap_in(SWAP_FILE, mmu->mem_ptr, (uint32_t)free_frame, page_index_in);
        addPage(mmu, page_index_in, free_frame);
#ifdef _TEST_
        printf("Page %u swaped in on frame %u\n", page_index_in, (uint32_t)free_frame);
#endif
    }
}

uint8_t SecondChance(MMU* mmu, uint32_t* page_index){
    PageTable pt = mmu->pt;

    while(1){
        // Return first page found that is not read and not written
        for(uint32_t i = mmu->cricular_list_index; i < PAGES; ++i){
            if(pt[i].valid && !(pt[i].unswappable) && !(pt[i].read) && !(pt[i].write)){
                // Update circular list index to the next element in the list
                mmu->cricular_list_index = i+1 == PAGES ? 0 : i+1;
                *page_index = i;
                return 0;
            }
        }

        // Return first page found that is not read but written
        // Set read bit to 0
        for(uint32_t i = 0; i < PAGES; ++i){
            if(pt[i].valid && !(pt[i].unswappable)){
                if(!(pt[i].read) && pt[i].write){
                    // Update circular list index to the next element in the list
                    mmu->cricular_list_index = i+1 == PAGES ? 0 : i+1;
                    *page_index = i;
                    return 1;
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

void removePage(MMU* mmu, uint32_t page_index){
    mmu->pt[page_index].frame_index = 0;
    mmu->pt[page_index].unswappable = 0;
    mmu->pt[page_index].valid = 0;
    mmu->pt[page_index].read = 0;
    mmu->pt[page_index].write = 0;
}

uint32_t getPhysicalAddress(MMU* mmu, uint32_t virt_addr){
    uint32_t page_offset = getOffset(virt_addr);
    uint32_t page_index = getPageIndex(virt_addr);

    uint32_t frame_index = mmu->pt[page_index].frame_index;
    uint32_t phys_addr = (frame_index << PAGE_OFFSET_SIZE) | page_offset;

    return phys_addr;
}

uint32_t getPageIndex(uint32_t virt_addr){ return virt_addr >> PAGE_OFFSET_SIZE; }

uint32_t getFrameIndex(uint32_t phys_addr){ return phys_addr >> PAGE_OFFSET_SIZE; }

uint32_t getOffset(uint32_t addr){
    uint32_t mask = (1 << PAGE_OFFSET_SIZE) - 1;
    return addr & mask;
}

AddressingResult AddressIsValid(MMU* mmu, uint32_t virt_addr){
    uint32_t page_index = getPageIndex(virt_addr);

    if(mmu->pt[page_index].unswappable) return Invalid;
    else if(mmu->pt[page_index].valid) return Valid;
    else return PageNotInMemory;
}

void PrintMMU(MMU mmu){
    uint32_t pages_for_data_structures = 1;
    uint32_t temp = (mmu.pt_len + (mmu.free_list.max_size * sizeof(int)));
    while((pages_for_data_structures * PAGE_SIZE) < temp)
        pages_for_data_structures++;
    
    // Page table address relative to phys_mem
    printf("MMU data\n");
    printf("______________________________\n");
    printf("Page table address ________ %ld\n", (((void*)mmu.pt) - ((void*)mmu.mem_ptr)));
    printf("Page table length _________ %u\n", mmu.pt_len);
    printf("Pages for data structures _ %u\n", pages_for_data_structures);
    printf("Circular list index _______ %u\n", mmu.cricular_list_index);
    // Free list address relative to phys_mem
    printf("Free list address _________ %ld\n", (((void*)mmu.free_list.list) - ((void*)mmu.mem_ptr)));
    printf("Free frames _______________ %d\n", mmu.free_list.size);
    printf("\nFree frames index: %d", mmu.free_list.start);
    for(uint32_t i = 0; i < mmu.free_list.max_size; ++i){
        if(mmu.free_list.list[i] != -1)
            printf(" | %d", mmu.free_list.list[i]);
    }
    printf("\n");
    // PrintArrayList(mmu.free_list);
    printf("______________________________\n\n");
}

void PrintPageTable(MMU mmu){
    printf("Page table content\n");
    printf("______________________________\n");
    printf("Page | Frame | V | U | R | W\n");
    for(uint32_t i = 0; i < PAGES; ++i){
        int frame_index = mmu.pt[i].valid ? mmu.pt[i].frame_index : -1;
        printf("%4u |  %4d | %1u | %1u | %1u | %1u\n", i, frame_index, mmu.pt[i].valid, mmu.pt[i].unswappable, mmu.pt[i].read, mmu.pt[i].write);
    }
    printf("______________________________\n\n");
}