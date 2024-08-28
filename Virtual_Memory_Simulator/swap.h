#pragma once

#include "VMSS_constants.h"

// Initialise swap file named swap_file_name containing VIRT_MEM_SIZE bytes.
void Swap_init(char* swap_file_name);

// Swap in the page with index page_index from swap area and copy it on frame with index frame_index
void Swap_in(char* swap_file_name, char* phys_mem, uint32_t frame_index, uint32_t page_index);

// Swap out the frame with index frame_index and copy it in swap area on page with index page_index
void Swap_out(char* swap_file_name, char* phys_mem, uint32_t frame_index, uint32_t page_index);

// Swap out the frame with index frame_index and copy it in swap area on page with index page_index_out
// Then swap in the page with index page_index_in from swap area and copy it on frame with index frame_index
void Swap_out_and_in(char* swap_file_name, char* phys_mem, uint32_t frame_index, uint32_t page_index_out, uint32_t page_index_in);