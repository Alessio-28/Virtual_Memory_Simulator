#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MMU.h"
#include "swap.h"

#define MAX_VIRT_MEM_SIZE_BIT 24
#define MAX_PHYS_MEM_SIZE_BIT 20
#define MAX_PAGE_SIZE_BIT     12

char phys_mem[PHYS_MEM_SIZE];

void check_macros();
void print_macros();
void phys_mem_init();
void print_phys_mem();



int main(int argc, char** argv){
    check_macros();
    print_macros();

    MMU mmu;
    MMU* mmu_ptr = &mmu;

    printf("Preparing physical memory\n");
    phys_mem_init();
    printf("Preparing MMU\n");
    MMU_init(mmu_ptr, phys_mem);
    printf("Preparing swap file\n");
    Swap_init(SWAP_FILE);

    char command[256];
    char input[256];
    while(command[0] != '0' && command[0] != 'Q' && command[0] != 'q'){
        uint32_t vms = 0;
        int virt_addr = 0;
        char* read_byte = 0;
        char write_byte[256];

        printf("\n");
        printf("Enter:\n");
        printf("1 - Read from page\n");
        printf("2 - Write on page\n");
        printf("3 - Print physical memory\n");
        printf("4 - Print MMU\n");
        printf("5 - Print page table\n");
        printf("6 - Print swap file\n");
        printf("7 - Random read/write on pages\n");
        printf("0 | Q | q - Quit\n");
        printf("Command: ");
        scanf("%s", command);
        printf("\n");

        switch(command[0]){
            case '1':
                vms = VIRT_MEM_SIZE-1;
                do{
                    printf("Enter address from which to read (0 <= address <= %u): ", vms);
                    scanf("%s", input);
                    virt_addr = atoi(input);
                }while(virt_addr < 0 || virt_addr > vms);
                read_byte = MMU_readByte(mmu_ptr, (uint32_t)virt_addr);
                if(read_byte) printf("Character read: %c\n", read_byte[0]);
                break;
            case '2':
                vms = VIRT_MEM_SIZE-1;
                do{
                    printf("Enter address on which to write (0 <= address <= %u): ", vms);
                    scanf("%s", input);
                    virt_addr = atoi(input);
                }while(virt_addr < 0 || virt_addr > vms);
                printf("Enter character to write: ");
                scanf("%s", write_byte);
                MMU_writeByte(mmu_ptr, (uint32_t)virt_addr, write_byte[0]);
                break;
            case '3':
                print_phys_mem();
                break;
            case '4':
                PrintMMU(mmu);
                break;
            case '5':
                PrintPageTable(mmu);
                break;
            case '6':
                PrintSwap(SWAP_FILE);
                break;
            case '7':
                printf("Nope\n");
                break;
            case '0':
            case 'Q':
            case 'q':
                printf("Exiting\n");
                break;
            default:
                printf("Command not recognised\n");
        }
    }

    Swap_unlink(SWAP_FILE);
    return 0;
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

void phys_mem_init(){ 
    memset(phys_mem, '0', PHYS_MEM_SIZE);
}

void print_phys_mem(){
    printf("Physical memory data\n_________________________\n");
    for(uint32_t i = 0; i < FRAMES; ++i){
        printf("Frame index: %4u - ", i);
        for(uint32_t j = 0; j < PAGE_SIZE; ++j)
            printf("%c", phys_mem[i*PAGE_SIZE + j]);
        printf("\n");
    }
    printf("_________________________\n\n");
}