#pragma once

#include "VMSS_constants.h"
#include <stdio.h>
#include <stdlib.h>

char* swap_file = "swap.txt";

void Swap_init(){
    FILE* f = fopen(swap_file, "w");
    assert(f && "Swap file descriptor NULL");

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