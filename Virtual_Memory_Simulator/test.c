#define _XOPEN_SOURCE 500

#include "test.h"
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
    uint32_t virt_mem_size_bit = VIRT_MEM_SIZE_BIT;
    uint32_t phys_mem_size_bit = PHYS_MEM_SIZE_BIT;
    uint32_t page_size_bit = PAGE_SIZE_BIT;
    uint32_t virt_mem_size = VIRT_MEM_SIZE;
    uint32_t phys_mem_size = PHYS_MEM_SIZE;
    uint32_t page_size = PAGE_SIZE;
    uint32_t pages = PAGES;
    uint32_t frames = FRAMES;

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

void phys_mem_init(char* mem){ memset(mem, '0', PHYS_MEM_SIZE); }

void print_phys_mem(MMU mmu, char* mem){
    printf("Physical memory data\n");
    printf("_______________________________________\n");
    printf("  Frame |   Page | V | U | R | W | Data\n");
    for(uint32_t i = 0; i < FRAMES; ++i){
        int page_index = 0;
        for(; page_index < PAGES; ++page_index){
            if(mmu.pt[page_index].frame_index == i)
                break;
        }
        if(page_index != PAGES)
            printf(" 0x%04x | 0x%04x | %1u | %1u | %1u | %1u | ", i, page_index, mmu.pt[page_index].valid, mmu.pt[page_index].unswappable, mmu.pt[page_index].read, mmu.pt[page_index].write);
        else
            printf(" 0x%04x |     -1 | 0 | 0 | 0 | 0 | ", i);
        
        char temp[PAGE_SIZE+1];
        strncpy(temp, mem+(i*PAGE_SIZE), PAGE_SIZE);
        temp[PAGE_SIZE] = '\0';
        printf("%s\n", temp);

        // for(uint32_t j = 0; j < PAGE_SIZE; ++j)
        //     printf("%d ", mem[i*PAGE_SIZE + j]);
        // printf("\n");
    }
    printf("_______________________________________\n\n");
}

void print_working_set(MMU mmu, char * mem){
    printf("Physical memory data\n");
    printf("__________________________________\n");
    printf("  Frame |   Page | V | U | R | W |\n");
    for(uint32_t i = 0; i < FRAMES; ++i){
        int page_index = 0;
        for(; page_index < PAGES; ++page_index){
            if(mmu.pt[page_index].frame_index == i)
                break;
        }
        if(page_index != PAGES)
            printf(" 0x%04x | 0x%04x | %1u | %1u | %1u | %1u |\n", i, page_index, mmu.pt[page_index].valid, mmu.pt[page_index].unswappable, mmu.pt[page_index].read, mmu.pt[page_index].write);
        else
            printf(" 0x%04x |     -1 | 0 | 0 | 0 | 0 |\n", i);
    }
    printf("__________________________________\n\n");
}

void read_from_page(MMU* mmu, int mem_limit){
    char input[256];
    int virt_addr = -1;
    char* read_byte;
    
    do{
        printf("Enter address from which to read (0x0 <= address <= 0x%x): ", mem_limit);
        scanf("%s", input);
        virt_addr = atoi(input);
    }while(virt_addr < 0 || virt_addr > mem_limit);

    read_byte = MMU_readByte(mmu, (uint32_t)virt_addr);
    if(read_byte) printf("Character read: %c\n", read_byte[0]);
}

void write_on_page(MMU* mmu, int mem_limit){
    char input[256];
    int virt_addr = -1;
    char write_byte[256];
    
    do{
        printf("Enter address on which to write (0x0 <= address <= 0x%x): ", mem_limit);
        scanf("%s", input);
        virt_addr = atoi(input);
    }while(virt_addr < 0 || virt_addr > mem_limit);

    printf("Enter string to write: ");
    scanf("%s", write_byte);

    for(uint32_t i = 0; write_byte[i] != '\0'; ++i){
        char wb = write_byte[i];
        MMU_writeByte(mmu, (uint32_t)(virt_addr + i), wb);
    }
}

void random_read_write(MMU* mmu, int mem_limit){
    char input[256];
    int n = -1;
    do{
        printf("How many read/write: ");
        scanf("%s", input);
        n = atoi(input);
    }while(n < 0);
    printf("\n");

    srandom((uint32_t)time(NULL));
    for(int i = 0; i < n; ++i){
        uint32_t rw = (uint32_t)(random() & 1);
        uint32_t virt_addr = (uint32_t)(random() % VIRT_MEM_SIZE);

        // 0 = read, 1 = write
        if(rw == 0){
            printf("Read from address: 0x%x\n", virt_addr);
            char* c = MMU_readByte(mmu, virt_addr);
            if(c) printf("Character read: %c\n", c[0]);
        }
        else{
            char k = (char)(random() % 26) + 'a';
            printf("Write character %c on address: 0x%x\n", k, virt_addr);
            MMU_writeByte(mmu, virt_addr, k);
        }
        printf("\n");
    }
}