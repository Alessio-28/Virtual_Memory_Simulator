#include "swap.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

void Swap_init(const char* swap_file_name){
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

void Swap_unlink(const char* swap_file_name){ unlink(swap_file_name); }

void Swap_in(const char* swap_file_name, char* phys_mem, const uint32_t frame_index, const uint32_t page_index){
    assert(swap_file_name && "Swap_file_name must not be NULL");
    assert(phys_mem && "Physical memory pointer must not be NULL");
    assert((frame_index < FRAMES) && "Frame index must be less than FRAMES");
    assert((page_index < PAGES) && "Page index must be less than PAGES");
    FILE* f = fopen(swap_file_name, "r");
    assert(f && "Swap file open failed");

    char buf[PAGE_SIZE+1];
    fseek(f, page_index*PAGE_SIZE, SEEK_SET);
    const char* ret = fgets(buf, PAGE_SIZE+1, f);

    fclose(f);

    assert(ret && "Swap in failed");

    memcpy(phys_mem+(frame_index*PAGE_SIZE), buf, PAGE_SIZE);
}

void Swap_out(const char* swap_file_name, char* phys_mem, const uint32_t frame_index, const uint32_t page_index){
    assert(swap_file_name && "Swap_file_name must not be NULL");
    assert(phys_mem && "Physical memory pointer must not be NULL");
    assert((frame_index < FRAMES) && "Frame index must be less than FRAMES");
    assert((page_index < PAGES) && "Page index must be less than PAGES");
    FILE* f = fopen(swap_file_name, "r+");
    assert(f && "Swap file open failed");

    char buf[PAGE_SIZE+1];
    fseek(f, page_index*PAGE_SIZE, SEEK_SET);
    memcpy(buf, phys_mem+(frame_index*PAGE_SIZE), PAGE_SIZE);
    buf[PAGE_SIZE] = '\0';

    const int res_out = fprintf(f, "%s", buf);
    
    fclose(f);
    
    if(res_out < 0){
        printf("Swap out failed");
        exit(EXIT_FAILURE);
    }
}

void Swap_out_and_in(const char* swap_file_name, char* phys_mem, const uint32_t frame_index, const uint32_t page_index_out, const uint32_t page_index_in){
    assert(swap_file_name && "Swap_file_name must not be NULL");
    assert(phys_mem && "Physical memory pointer must not be NULL");
    assert((frame_index < FRAMES) && "Frame index must be less than FRAMES");
    assert((page_index_out < PAGES && page_index_in < PAGES) && "Page index must be less than PAGES");

    Swap_out(swap_file_name, phys_mem, frame_index, page_index_out);
    Swap_in(swap_file_name, phys_mem, frame_index, page_index_in);
}

//_______________________________________________________________________________________________________________________________
//_______________________________________________________________________________________________________________________________
//_______________________________________________________________________________________________________________________________

void PrintSwap(const char* swap_file_name){
    assert(swap_file_name && "Swap_file_name must not be NULL");
    FILE* f = fopen(swap_file_name, "r");
    assert(f && "Swap file open failed");

    char buf[PAGE_SIZE+1];
    printf("Swap file data\n_________________________\n");
    for(uint32_t i = 0; i < PAGES && fgets(buf, PAGE_SIZE+1, f) != NULL; ++i)
        printf("Page index 0x%04x - %s\n", i, buf);
    printf("_________________________\n\n");

    if(ferror(f)){
        fclose(f);
        printf("PrintSwap failed\n");
        exit(EXIT_FAILURE);
    }
    
    fclose(f);
}

void PrintPageInSwap(const char* swap_file_name, const uint32_t page_index){
    assert(swap_file_name && "Swap_file_name must not be NULL");
    assert((page_index < PAGES) && "page_index must be less than PAGES");
    FILE* f = fopen(swap_file_name, "r");
    assert(f && "Swap file open failed");

    char buf[PAGE_SIZE+1];
    fseek(f, page_index*PAGE_SIZE, SEEK_SET);
    const char* res = fgets(buf, PAGE_SIZE+1, f);

    fclose(f);

    if(res == NULL){
        printf("PrintPageInSwap failed\n");
        exit(EXIT_FAILURE);
    }

    printf("Page index 0x%x - %s\n", page_index, buf);
}