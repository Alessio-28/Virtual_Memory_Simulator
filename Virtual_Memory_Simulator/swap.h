#pragma once

#include "VMSS_constants.h"

// Initialise swap file named swap_file_name containing size bytes.
void Swap_init(char* swap_file_name);

void Swap_in(char* swap_file_name, char* phys_mem, uint32_t frame_index, uint32_t page_index);
void Swap_out_and_in(char* swap_file_name, char* phys_mem, uint32_t frame_index, uint32_t page_index_in, uint32_t page_index_out);