#include "MMU.h"
#include "swap.h"
#include <stdio.h>
#include <string.h>
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
    const uint32_t temp = (mmu->pt_len + (mmu->free_list.max_size * sizeof(int)));
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


int MMU_writeByte(MMU* mmu, const uint32_t virt_addr, const char c){
    assert((virt_addr >= 0 || virt_addr < VIRT_MEM_SIZE) && "Virtual address must be non negative and less than VIRT_MEM_SIZE");
    const AddressingResult res = AddressIsValid(mmu, virt_addr);

    if(res == Invalid)
        return -1;

    if(res == PageNotInMemory)
        MMU_exception(mmu, virt_addr);

    const uint32_t page_index = getPageIndex(virt_addr);
    mmu->pt[page_index].read  = 1;
    mmu->pt[page_index].write = 1;
    const uint32_t phys_addr = getPhysicalAddress(mmu, virt_addr);

    mmu->mem_ptr[phys_addr] = c;

#ifdef _TEST_
    printf("Character written on page 0x%x on frame 0x%x\n", page_index, getFrameIndex(phys_addr));
#endif

    return 0;
}

const char* MMU_readByte(MMU* mmu, const uint32_t virt_addr){
    assert((virt_addr >= 0 || virt_addr < VIRT_MEM_SIZE) && "Virtual address must be non negative and less than VIRT_MEM_SIZE");
    const AddressingResult res = AddressIsValid(mmu, virt_addr);

    if(res == Invalid)
        return NULL;

    if(res == PageNotInMemory)
        MMU_exception(mmu, virt_addr);

    const uint32_t page_index = getPageIndex(virt_addr);
    mmu->pt[page_index].read = 1;
    const uint32_t phys_addr = getPhysicalAddress(mmu, virt_addr);

#ifdef _TEST_
    printf("Character read from page 0x%x on frame 0x%x\n", page_index, getFrameIndex(phys_addr));
#endif

    return &(mmu->mem_ptr[phys_addr]);
}

void MMU_exception(MMU* mmu, const uint32_t virt_addr){
    const uint32_t page_index_in = getPageIndex(virt_addr);
    const int free_frame = getElement(&(mmu->free_list));
#ifdef _TEST_
    printf("--PAGE FAULT--\n");
#endif
    if(free_frame == -1){
        uint32_t page_index_out;
        const uint8_t class = SecondChance(mmu, &page_index_out);
        const uint32_t frame_index = mmu->pt[page_index_out].frame_index;
        // Swap_out_and_in(SWAP_FILE, mmu->mem_ptr, frame_index, page_index_out, page_index_in);

        if(class == 1){
            Swap_out(SWAP_FILE, mmu->mem_ptr, frame_index, page_index_out);
#ifdef _TEST_
            printf("Page 0x%x swapped out from frame 0x%x\n", page_index_out, frame_index);
#endif
        }

        Swap_in(SWAP_FILE, mmu->mem_ptr, frame_index, page_index_in);
        removePage(mmu, page_index_out);
        addPage(mmu, page_index_in, frame_index);
#ifdef _TEST_
        if(class == 1)
            printf("Page 0x%x swapped in on frame 0x%x\n", page_index_in, frame_index);
        else
            printf("Page 0x%x swapped in on frame 0x%x in place of page 0x%x\n", page_index_in, frame_index, page_index_out);
#endif
    }
    else{
        Swap_in(SWAP_FILE, mmu->mem_ptr, (uint32_t)free_frame, page_index_in);
        addPage(mmu, page_index_in, free_frame);
#ifdef _TEST_
        printf("Page 0x%x swapped in on frame 0x%x\n", page_index_in, (uint32_t)free_frame);
#endif
    }
}

uint8_t SecondChance(MMU* mmu, uint32_t* page_index){
    PageTable pt = mmu->pt;
    uint32_t CLI = mmu->cricular_list_index;

    uint32_t i;
    uint32_t j;
    while(1){

        // Return first page found that is not read and not written
        for(i = 0, j = CLI; i < PAGES; ++i, ++j){
            if(j == PAGES) j = 0;

            if(pt[j].valid && !(pt[j].unswappable) && !(pt[j].read) && !(pt[j].write)){
                // Update circular list index to the next element in the list
                mmu->cricular_list_index = j+1 == PAGES ? 0 : j+1;
                *page_index = j;
                return 0;
            }
        }

        // Return first page found that is not read but written
        // Set read bit to 0
        for(i = 0, j = CLI; i < PAGES; ++i, ++j){
            if(j == PAGES) j = 0;

            if(pt[j].valid && !(pt[j].unswappable)){
                if(!(pt[j].read) && pt[j].write){
                    // Update circular list index to the next element in the list
                    mmu->cricular_list_index = j+1 == PAGES ? 0 : j+1;
                    *page_index = j;
                    return 1;
                }
                pt[j].read = 0;
            }
        }
    }
}

void addPage(MMU* mmu, const uint32_t page_index, const uint32_t frame_index){
    mmu->pt[page_index].frame_index = frame_index;
    mmu->pt[page_index].unswappable = 0;
    mmu->pt[page_index].valid = 1;
    mmu->pt[page_index].read = 0;
    mmu->pt[page_index].write = 0;
}

void removePage(MMU* mmu, const uint32_t page_index){
    mmu->pt[page_index].frame_index = 0;
    mmu->pt[page_index].unswappable = 0;
    mmu->pt[page_index].valid = 0;
    mmu->pt[page_index].read = 0;
    mmu->pt[page_index].write = 0;
}

uint32_t getPhysicalAddress(const MMU* mmu, const uint32_t virt_addr){
    uint32_t page_offset = getOffset(virt_addr);
    uint32_t page_index = getPageIndex(virt_addr);

    uint32_t frame_index = mmu->pt[page_index].frame_index;
    uint32_t phys_addr = (frame_index << PAGE_OFFSET_SIZE) | page_offset;

    return phys_addr;
}

uint32_t getPageIndex(const uint32_t virt_addr){ return virt_addr >> PAGE_OFFSET_SIZE; }

uint32_t getFrameIndex(const uint32_t phys_addr){ return phys_addr >> PAGE_OFFSET_SIZE; }

uint32_t getOffset(const uint32_t addr){
    uint32_t mask = (1 << PAGE_OFFSET_SIZE) - 1;
    return addr & mask;
}

AddressingResult AddressIsValid(const MMU* mmu, const uint32_t virt_addr){
    uint32_t page_index = getPageIndex(virt_addr);

    if(mmu->pt[page_index].unswappable) return Invalid;
    else if(mmu->pt[page_index].valid) return Valid;
    else return PageNotInMemory;
}

//_______________________________________________________________________________________________________________________________
//_______________________________________________________________________________________________________________________________
//_______________________________________________________________________________________________________________________________

void PrintMMU(const MMU mmu){
    uint32_t pages_for_data_structures = 1;
    const uint32_t temp = (mmu.pt_len + (mmu.free_list.max_size * sizeof(int)));
    while((pages_for_data_structures * PAGE_SIZE) < temp)
        pages_for_data_structures++;
    
    // Page table address relative to phys_mem
    // Free list address relative to phys_mem
    const char* str = "MMU data\n"
                      "______________________________\n"
                      "Page table address ________ 0x%lx\n"
                      "Page table length _________ %u bytes\n"
                      "Pages for data structures _ %u\n"
                      "Circular list index _______ 0x%x\n"
                      "Free list address _________ 0x%lx\n"
                      "Free frames _______________ %d\n";

    printf(str,
            (((void*)mmu.pt) - ((void*)mmu.mem_ptr)),
            mmu.pt_len,
            pages_for_data_structures,
            mmu.cricular_list_index,
            (((void*)mmu.free_list.list) - ((void*)mmu.mem_ptr)),
            mmu.free_list.size
          );
           
    if(mmu.free_list.start != -1){
        printf("\nFree frames index: 0x%x", mmu.free_list.start);
        for(uint32_t i = 0; i < mmu.free_list.max_size; ++i){
            if(mmu.free_list.list[i] != -1)
                printf(" | 0x%x", mmu.free_list.list[i]);
        }
        printf("\n");
    }
    // PrintArrayList(mmu.free_list);
    printf("______________________________\n\n");
}

void PrintPageTable(const MMU mmu){
    int frame_index = -1;
    const char* str = "Page table content\n"
                      "__________________________________\n"
                      "  Page |   Frame | V | U | R | W\n";
    
    printf(str);
    for(uint32_t i = 0; i < PAGES; ++i){
        frame_index = mmu.pt[i].valid ? mmu.pt[i].frame_index : -1;
        printf("0x%4x |  0x%4x | %1u | %1u | %1u | %1u\n", i, frame_index, mmu.pt[i].valid, mmu.pt[i].unswappable, mmu.pt[i].read, mmu.pt[i].write);
    }
    printf("__________________________________\n\n");
}

void PrintPhysicalMemory(const MMU mmu){
    const char* str = "Physical memory data\n"
                      "_______________________________________\n"
                      "  Frame |   Page | V | U | R | W | Data\n";
    printf(str);
    for(uint32_t i = 0; i < FRAMES; ++i){
        int page_index = 0;
        for(; page_index < PAGES; ++page_index){
            if(mmu.pt[page_index].frame_index == i)
                break;
        }
        
        if(page_index != PAGES){
            Page p = mmu.pt[page_index];
            printf(" 0x%04x | 0x%04x | %1u | %1u | %1u | %1u | ", i, page_index, p.valid, p.unswappable, p.read, p.write);
        }
        else
            printf(" 0x%04x |     -1 | 0 | 0 | 0 | 0 | ", i);
        
        char temp[PAGE_SIZE+1];
        memcpy(temp, mmu.mem_ptr+(i*PAGE_SIZE), PAGE_SIZE);
        temp[PAGE_SIZE] = '\0';
        printf("%s\n", temp);

        // for(uint32_t j = 0; j < PAGE_SIZE; ++j)
        //     printf("%d ", phys_mem[i*PAGE_SIZE + j]);
        // printf("\n");
    }
    printf("_______________________________________\n\n");
}

void PrintWorkingSet(const MMU mmu){
    const char* str = "Working set data\n"
                      "__________________________________\n"
                      "  Frame |   Page | V | U | R | W |\n";
    printf(str);
    for(uint32_t i = 0; i < FRAMES; ++i){
        int page_index = 0;
        for(; page_index < PAGES; ++page_index){
            if(mmu.pt[page_index].frame_index == i)
                break;
        }
        if(page_index != PAGES){
            Page p = mmu.pt[page_index];
            printf(" 0x%04x | 0x%04x | %1u | %1u | %1u | %1u |\n", i, page_index, p.valid, p.unswappable, p.read, p.write);
        }
    }
    printf("__________________________________\n\n");
}

void PrintPageInMemory(const MMU mmu, const uint32_t page_index){
    const Page p = mmu.pt[page_index];
    if(p.valid){
        printf("Page requested not in main memory\n");
        return;
    }

    char buf[PAGE_SIZE+1];
    memcpy(buf, mmu.mem_ptr+(p.frame_index*PAGE_SIZE), PAGE_SIZE);
    buf[PAGE_SIZE] = '\0';

    const char* str = "__________________________________\n"
                      "   Page |  Frame | V | U | R | W |\n"
                      " 0x%04x | 0x%04x | %1u | %1u | %1u | %1u |\n"
                      "%s\n"
                      "__________________________________\n\n";

    printf(str, page_index, p.frame_index, p.valid, p.unswappable, p.read, p.write, buf);
}