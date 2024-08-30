#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "MMU.h"
#include "swap.h"

#define MAX_VIRT_MEM_SIZE_BIT 24
#define MAX_PHYS_MEM_SIZE_BIT 20
#define MAX_PAGE_SIZE_BIT     12

char phys_mem[PHYS_MEM_SIZE];

void check_macros();
void print_macros();
void phys_mem_init(char* mem);
void print_phys_mem(MMU mmu, char* mem);

void read_from_page(MMU* mmu, int mem_limit);
void write_on_page(MMU* mmu, int mem_limit);

void random_read_write(MMU* mmu, int mem_limit){
    char input[256];
    int n = -1;
    do{
        printf("How many read/write: ");
        scanf("%s", input);
        n = atoi(input);
    }while(n < 0);

    srandom((uint32_t)time(NULL));
    for(int i = 0; i < n; ++i){
        uint32_t rw = (uint32_t)(random() & 1);
        uint32_t virt_addr = (uint32_t)(random() % VIRT_MEM_SIZE);

        // 0 = read, 1 = write
        if(rw == 0){
            printf("Read from address: %u\n", virt_addr);
            char* c = MMU_readByte(mmu, virt_addr);
            if(c) printf("Character read: %c\n\n", c[0]);
        }
        else{
            char k = (char)(random() % 26) + 'a';
            printf("Write character %c on address: %u\n\n", k, virt_addr);
            MMU_writeByte(mmu, virt_addr, k);
        }
    }
}

int main(int argc, char** argv){
    check_macros();
    print_macros();

    MMU mmu;
    MMU* mmu_ptr = &mmu;

    printf("Preparing physical memory\n");
    phys_mem_init(phys_mem);
    printf("Preparing MMU\n");
    MMU_init(mmu_ptr, phys_mem);
    printf("Preparing swap file\n");
    Swap_init(SWAP_FILE);

    char command[256];
    uint32_t vms = VIRT_MEM_SIZE-1;
    while(command[0] != '0' && command[0] != 'Q' && command[0] != 'q'){

        printf("\n");
        printf("Enter:\n");
        printf("1 - Read from page\n");
        printf("2 - Write on page\n");
        printf("3 - Print physical memory\n");
        printf("4 - Print swap file\n");
        printf("5 - Print MMU\n");
        printf("6 - Print macros\n");
        printf("7 - Random read/write on pages\n");
        printf("0 | Q | q - Quit\n");
        printf("Command: ");
        scanf("%s", command);
        printf("\n");

        if(strlen(command) > 1) command[0] = 0;

        switch(command[0]){
            case '1':
                read_from_page(mmu_ptr, vms);
                break;
            case '2':
                write_on_page(mmu_ptr, vms);
                break;
            case '3':
                print_phys_mem(mmu, phys_mem);
                break;
            case '4':
                PrintSwap(SWAP_FILE);
                break;
            case '5':
                PrintMMU(mmu);
                break;
            case '6':
                print_macros();
                break;
            case '7':
                random_read_write(mmu_ptr, vms);
                break;
            case '0':
            case 'Q':
            case 'q':
                printf("Exiting\n");
                break;
            default:
                printf("Invalid Command\n");
        }
    }

    Swap_unlink(SWAP_FILE);
    return 0;
}

void read_from_page(MMU* mmu, int mem_limit){
    char input[256];
    int virt_addr = -1;
    char* read_byte;
    
    do{
        printf("Enter address from which to read (0 <= address <= %u): ", mem_limit);
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
        printf("Enter address on which to write (0 <= address <= %u): ", mem_limit);
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

//____________________________________________________________________________
//____________________________________________________________________________
//____________________________________________________________________________

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

    if(VIRT_MEM_SIZE_BIT <= 0 || PHYS_MEM_SIZE_BIT <= 0 || PAGE_SIZE_BIT <= 0){
        printf("PAGE_SIZE_BIT must be less than or equal to 12\n");
        exit(EXIT_FAILURE);
    }

    if(VIRT_MEM_SIZE_BIT < PHYS_MEM_SIZE_BIT || PHYS_MEM_SIZE_BIT < PAGE_SIZE_BIT){
        printf("VIRT_MEM_SIZE_BIT must be greater than or equal to PHYS_MEM_SIZE_BIT\n");
        printf("PHYS_MEM_SIZE_BIT must be greater than or equal to PAGE_SIZE_BIT\n");
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
    printf("Frame | Page | V | U | R | W | Data\n");
    for(uint32_t i = 0; i < FRAMES; ++i){
        int page_index = 0;
        for(; page_index < PAGES; ++page_index){
            if(mmu.pt[page_index].frame_index == i)
                break;
        }
        if(page_index != PAGES)
            printf(" %4u | %4d | %1u | %1u | %1u | %1u | ", i, page_index, mmu.pt[page_index].valid, mmu.pt[page_index].unswappable, mmu.pt[page_index].read, mmu.pt[page_index].write);
        else
            printf(" %4u |   -1 | 0 | 0 | 0 | 0 | ", i);
        
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