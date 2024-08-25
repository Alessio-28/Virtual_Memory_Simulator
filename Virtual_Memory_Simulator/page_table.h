#pragma once

#include <VMSS_constants.h>

typedef struct Page{
    uint32_t frame_index: FRAME_INDEX_SIZE;
    uint8_t unswappable: 1;
    uint8_t valid: 1;
    uint8_t read: 1;
    uint8_t write: 1;
} Page;

typedef struct PageTable{
    Page table[PAGES];
    
} PageTable;