#include "swap.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

void Swap_init(char* swap_file_name){
    assert(swap_file_name && "Swap_file_name must not be NULL");
    FILE* f = fopen(swap_file_name, "w");
    assert(f && "Swap file open failed");

    char temp[PAGE_SIZE+1];
    memset(temp, '0', PAGE_SIZE);
    temp[PAGE_SIZE] = '\0';

    for(uint32_t i = 0; i < PAGES; ++i){
        int ret = fprintf(f, "%s", temp);
        
        if(ret != PAGE_SIZE){
            printf("Swap file init failed\n");
            exit(EXIT_FAILURE);
        }
    }

    fclose(f);
}

void Swap_unlink(char* swap_file_name){ unlink(swap_file_name); }

void Swap_in(char* swap_file_name, char* phys_mem, uint32_t frame_index, uint32_t page_index){
    assert(swap_file_name && "Swap_file_name must not be NULL");
    assert(phys_mem && "Physical memory pointer must not be NULL");
    assert((frame_index < FRAMES) && "Frame index must be less than FRAMES");
    assert((page_index < PAGES) && "Page index must be less than PAGES");
    FILE* f = fopen(swap_file_name, "r");
    assert(f && "Swap file open failed");

    char buf[PAGE_SIZE+1];
    fseek(f, page_index*PAGE_SIZE, SEEK_SET);
    char* ret = fgets(buf, PAGE_SIZE+1, f);

    fclose(f);

    assert(ret && "Swap in failed");

    strncpy(phys_mem+(frame_index*PAGE_SIZE), buf, PAGE_SIZE);
}

void Swap_out(char* swap_file_name, char* phys_mem, uint32_t frame_index, uint32_t page_index){
    assert(swap_file_name && "Swap_file_name must not be NULL");
    assert(phys_mem && "Physical memory pointer must not be NULL");
    assert((frame_index < FRAMES) && "Frame index must be less than FRAMES");
    assert((page_index < PAGES) && "Page index must be less than PAGES");
    FILE* f = fopen(swap_file_name, "r+");
    assert(f && "Swap file open failed");

    char buf[PAGE_SIZE+1];
    fseek(f, page_index*PAGE_SIZE, SEEK_SET);
    strncpy(buf, phys_mem+(frame_index*PAGE_SIZE), PAGE_SIZE);
    buf[PAGE_SIZE] = '\0';

    int res_out = fprintf(f, "%s", buf);
    
    fclose(f);
    
    if(res_out < 0){
        printf("Swap out failed");
        exit(EXIT_FAILURE);
    }
}

void Swap_out_and_in(char* swap_file_name, char* phys_mem, uint32_t frame_index, uint32_t page_index_out, uint32_t page_index_in){
    assert(swap_file_name && "Swap_file_name must not be NULL");
    assert(phys_mem && "Physical memory pointer must not be NULL");
    assert((frame_index < FRAMES) && "Frame index must be less than FRAMES");
    assert((page_index_out < PAGES && page_index_in < PAGES) && "Page index must be less than PAGES");

    Swap_out(swap_file_name, phys_mem, frame_index, page_index_out);
    Swap_in(swap_file_name, phys_mem, frame_index, page_index_in);
}

void PrintSwap(char* swap_file_name){
    assert(swap_file_name && "Swap_file_name must not be NULL");
    FILE* f = fopen(swap_file_name, "r");
    assert(f && "Swap file open failed");

    uint32_t buf_len = PAGE_SIZE+1;
    char buf[buf_len];
    printf("Swap file data\n_________________________\n");
    for(uint32_t i = 0; i < PAGES && fgets(buf, buf_len, f) != NULL; ++i)
        printf("Page index 0x%04x - %s\n", i, buf);
    printf("_________________________\n\n");

    if(ferror(f)){
        fclose(f);
        printf("PrintSwap failed\n");
        exit(EXIT_FAILURE);
    }
    
    fclose(f);
}