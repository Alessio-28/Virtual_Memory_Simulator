#include "MMU.h"

#define MAX_VIRT_MEM_SIZE_BIT 24
#define MAX_PHYS_MEM_SIZE_BIT 20
#define MAX_PAGE_SIZE_BIT     12

void check_macros();
void print_macros();
void phys_mem_init(char* phys_mem);

void print_page_in_memory(MMU mmu);
void print_swap();

void read_from_page(MMU* mmu);
void write_on_page(MMU* mmu);

void random_read_write(MMU* mmu);