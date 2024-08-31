#include "MMU.h"

#define MAX_VIRT_MEM_SIZE_BIT 24
#define MAX_PHYS_MEM_SIZE_BIT 20
#define MAX_PAGE_SIZE_BIT     12

void check_macros();
void print_macros();
void phys_mem_init(char* phys_mem);
// Printf pages in physical memory with their info and data
void print_phys_mem(MMU mmu, char* phys_mem);
// Printf pages in physical memory with their info
void print_working_set(MMU mmu, char* phys_mem);

void read_from_page(MMU* mmu, int mem_limit);
void write_on_page(MMU* mmu, int mem_limit);

void random_read_write(MMU* mmu, int mem_limit);