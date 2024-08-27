#pragma once

#include "swap.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

void Swap_init(char* swap_file_name){
    assert(swap_file_name && "Swap_file_name must not be NULL");
    FILE* f = fopen(swap_file_name, "w");
    assert(f && "Swap file open failed");

    char temp[VIRT_MEM_SIZE+1];
    for(uint32_t i = 0; i < VIRT_MEM_SIZE+1; ++i)
        temp[i] = '0';
    temp[VIRT_MEM_SIZE] = '\0';

    int ret = fprintf(f, "%s", temp);

    fclose(f);

    if(ret != VIRT_MEM_SIZE){
        printf("Swap file init failed\n");
        exit(EXIT_FAILURE);
    }
}

void Swap_in(char* swap_file_name, char* phys_mem, uint32_t frame_index, uint32_t page_index){
    assert(swap_file_name && "Swap_file_name must not be NULL");
    FILE* f = fopen(swap_file_name, "r");
    assert(f && "Swap file open failed");

    char buf[PAGE_SIZE+1];

    fseek(f, page_index*PAGE_SIZE, SEEK_SET);

    char* ret = fgets(buf, PAGE_SIZE+1, f);

    fclose(f);

    assert(ret && "Swap in failed");

    strncpy(phys_mem+(frame_index*PAGE_SIZE), buf, PAGE_SIZE);
}

void Swap_out_and_in(char* swap_file_name, char* phys_mem, uint32_t frame_index, uint32_t page_index_out, uint32_t page_index_in){
    assert(swap_file_name && "Swap_file_name must not be NULL");
    FILE* f = fopen(swap_file_name, "r+");
    assert(f && "Swap file open failed");

    char buf[PAGE_SIZE+1];
    fseek(f, page_index_out*PAGE_SIZE, SEEK_SET);

    strncpy(buf, phys_mem+(frame_index*PAGE_SIZE), PAGE_SIZE);

    int res_out = fprintf(f, "%s", buf);
    if(res_out < 0){
        fclose(f);
        printf("Swap out failed");
        exit(EXIT_FAILURE);
    }

    fseek(f, page_index_in*PAGE_SIZE, SEEK_SET);

    char* res_in = fgets(buf, PAGE_SIZE+1, f);
    
    fclose(f);

    assert(res_in && "Swap in failed");

    strncpy(phys_mem+(frame_index*PAGE_SIZE), buf, PAGE_SIZE);
}