#define _XOPEN_SOURCE 500

#include "test.h"
#include "swap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void check_macros(){
    if(VIRT_MEM_SIZE_BIT > MAX_VIRT_MEM_SIZE_BIT){
        printf("VIRT_MEM_SIZE_BIT must be less than or equal to 24\n");
        exit(EXIT_FAILURE);
    }

    if(PHYS_MEM_SIZE_BIT > MAX_PHYS_MEM_SIZE_BIT){
        printf("PHYS_MEM_SIZE_BIT must be less than or equal to 20\n");
        exit(EXIT_FAILURE);
    }

    if(PAGE_SIZE_BIT > MAX_PAGE_SIZE_BIT){
        printf("PAGE_SIZE_BIT must be less than or equal to 12\n");
        exit(EXIT_FAILURE);
    }

    if(VIRT_MEM_SIZE_BIT < PHYS_MEM_SIZE_BIT || PHYS_MEM_SIZE_BIT < PAGE_SIZE_BIT){
        printf("VIRT_MEM_SIZE_BIT must be greater than or equal to PHYS_MEM_SIZE_BIT\n");
        printf("PHYS_MEM_SIZE_BIT must be greater than or equal to PAGE_SIZE_BIT\n");
        exit(EXIT_FAILURE);
    }

    if(VIRT_MEM_SIZE_BIT <= 0 || PHYS_MEM_SIZE_BIT <= 0 || PAGE_SIZE_BIT <= 0){
        printf("PAGE_SIZE_BIT must be less than or equal to 12\n");
        exit(EXIT_FAILURE);
    }
}

void print_macros(){
    const uint32_t virt_mem_size_bit = VIRT_MEM_SIZE_BIT;
    const uint32_t phys_mem_size_bit = PHYS_MEM_SIZE_BIT;
    const uint32_t page_size_bit = PAGE_SIZE_BIT;
    const uint32_t virt_mem_size = VIRT_MEM_SIZE;
    const uint32_t phys_mem_size = PHYS_MEM_SIZE;
    const uint32_t page_size = PAGE_SIZE;
    const uint32_t pages = PAGES;
    const uint32_t frames = FRAMES;

    printf("______________________________________\n");
    printf("Virtual address size __ %u bits\n", virt_mem_size_bit);
    printf("Physical address size _ %u bits\n", phys_mem_size_bit);
    printf("Page offset size ______ %u bits\n", page_size_bit);
    printf("Virtual memory size ___ %u bytes\n", virt_mem_size);
    printf("Physical memory size __ %u bytes\n", phys_mem_size);
    printf("Page size _____________ %u bytes\n", page_size);
    printf("Pages _________________ %u\n", pages);
    printf("Frames ________________ %u\n", frames);
    printf("______________________________________\n\n");
}

void phys_mem_init(char* phys_mem){ memset(phys_mem, '0', PHYS_MEM_SIZE); }

void print_page_in_memory(const MMU mmu){
    const uint32_t mem_limit = PAGES-1;
    uint32_t page_index = PAGES;
    
    do{
        printf("Enter index of page to print (0x0 <= address <= 0x%x): ", mem_limit);
        if(scanf("%x", &page_index) != 1) continue;
    }while(page_index > mem_limit);
    
    PrintPageInMemory(mmu, page_index);
}

void print_swap(){
    char input[256];
    
    do{
        printf("Print all swap file content?[y/n]: ");
        if(scanf("%s", input) != 1) continue;
    }while(input[0] != 'y' && input[0] != 'n');

    if(input[0] == 'y')
        PrintSwap(SWAP_FILE);
    else{
        const uint32_t page_index_limit = PAGES-1;
        uint32_t page_index = PAGES;
        do{
            printf("Enter index of page to print (0x0 <= index <= 0x%x): ", page_index_limit);
            if(scanf("%x", &page_index) != 1) continue;
        }while(page_index > page_index_limit);
        PrintPageInSwap(SWAP_FILE, page_index);
    }
}

void read_from_page(MMU* mmu){
    const uint32_t mem_limit = VIRT_MEM_SIZE-1;
    uint32_t virt_addr = VIRT_MEM_SIZE;
    
    do{
        printf("Enter address from which to read (0x0 <= address <= 0x%x): ", mem_limit);
        if(scanf("%x", &virt_addr) != 1) continue;
    }while(virt_addr > mem_limit);

    const char* read_byte = MMU_readByte(mmu, virt_addr);
    if(read_byte)
        printf("Character read: '%c'\n", read_byte[0]);
    else
        printf("Invalid Address\n");
}

void write_on_page(MMU* mmu){
    const uint32_t mem_limit = VIRT_MEM_SIZE-1;
    uint32_t virt_addr = VIRT_MEM_SIZE;
    char write_byte[256];

    do{
        printf("Enter address on which to write (0x0 <= address <= 0x%x): ", mem_limit);
        if(scanf("%x", &virt_addr) != 1) continue;
    }while(virt_addr > mem_limit);

    do{
        printf("Enter string to write: ");
    }while(scanf("%s", write_byte) != 1);

    const uint32_t len = (uint32_t)strlen(write_byte);
    for(uint32_t i = 0; i < len; ++i, ++virt_addr)
        MMU_writeByte(mmu, virt_addr, write_byte[i]);
}

void random_read_write(MMU* mmu){
    int input;
    do{
        printf("How many read/write: ");
        if(scanf("%d", &input) != 1) continue;
    }while(input < 0);
    printf("\n");

    srandom((uint32_t)time(NULL));
    for(int i = 0; i < input; ++i){
        uint32_t rw = (uint32_t)(random() & 1);

        uint32_t virt_addr = (uint32_t)(random() % VIRT_MEM_SIZE);
        // printf("virt_addr: 0x%x, page_index: 0x%x, offset: 0x%x\n", virt_addr, getPageIndex(virt_addr), getOffset(virt_addr));

        // 0 = read, 1 = write
        if(rw == 0){
            printf("Read from address: 0x%x\n", virt_addr);
            const char* read_byte = MMU_readByte(mmu, virt_addr);
            if(read_byte)
                printf("Character read: '%c'\n", read_byte[0]);
            else
                printf("Invalid Address\n");
        }
        else{
            char write_byte = (char)(random() % 26) + 'a';
            printf("Write character '%c' on address: 0x%x\n", write_byte, virt_addr);
            MMU_writeByte(mmu, virt_addr, write_byte);
        }
        printf("\n");
    }
}