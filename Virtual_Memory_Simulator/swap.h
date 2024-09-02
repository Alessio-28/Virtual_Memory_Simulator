#pragma once

#include "VMSS_constants.h"

// Initialise swap file named swap_file_name containing VIRT_MEM_SIZE bytes.
void Swap_init(const char* swap_file_name);
void Swap_unlink(const char* swap_file_name);

// Swap in the page with index page_index from swap area and copy it on frame with index frame_index
void Swap_in(const char* swap_file_name, char* phys_mem, const uint32_t frame_index, const uint32_t page_index);

// Swap out the frame with index frame_index and copy it in swap area on page with index page_index
void Swap_out(const char* swap_file_name, char* phys_mem, const uint32_t frame_index, const uint32_t page_index);

// Swap out the frame with index frame_index and copy it in swap area on page with index page_index_out
// Then swap in the page with index page_index_in from swap area and copy it on frame with index frame_index
void Swap_out_and_in(const char* swap_file_name, char* phys_mem, const uint32_t frame_index, const uint32_t page_index_out, const uint32_t page_index_in);

void PrintSwap(const char* swap_file_name);
void PrintPageInSwap(const char* swap_file_name, const uint32_t page_index);