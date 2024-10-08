#include <stdio.h>
#include <string.h>
#include "MMU.h"
#include "swap.h"
#include "test.h"

char phys_mem[PHYS_MEM_SIZE];

int main(){
    check_macros();
    print_macros();

    MMU mmu;
    MMU* mmu_ptr = &mmu;

    phys_mem_init(phys_mem);
    MMU_init(mmu_ptr, phys_mem);
    Swap_init(SWAP_FILE);

    char command[256];
    command[0] = 0;
    while(command[0] != '0' && command[0] != 'Q' && command[0] != 'q'){
        printf("\n");
        printf("Enter:\n");
        printf("1 - Read from page\n");
        printf("2 - Write on page\n");
        printf("3 - Print physical memory\n");
        printf("4 - Print working set\n");
        printf("5 - Print a page in main memory\n");
        printf("6 - Print swap file\n");
        printf("7 - Print MMU\n");
        printf("8 - Print macros\n");
        printf("9 - Random read/write on pages\n");
        printf("0 | Q | q - Quit\n");
        do{
            printf("Command: ");
        }while(scanf("%s", command) != 1);
        printf("\n");

        if(strlen(command) > 1) command[0] = 0;

        switch(command[0]){
            case '1':
                read_from_page(mmu_ptr);
                break;
            case '2':
                write_on_page(mmu_ptr);
                break;
            case '3':
                PrintPhysicalMemory(mmu);
                break;
            case '4':
                PrintWorkingSet(mmu);
                break;
            case '5':
                print_page_in_memory(mmu);
                break;
            case '6':
                print_swap();
                break;
            case '7':
                PrintMMU(mmu);
                break;
            case '8':
                print_macros();
                break;
            case '9':
                random_read_write(mmu_ptr);
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