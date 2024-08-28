#pragma once

typedef struct ArrayList{
    int* list;
    int start;
    int size;
    int max_size;
} ArrayList;

// Initialise array list, filling it from first (included) to last (included)
void ArrayList_init(ArrayList* array_list, int first, int last, int size);

void EmptyArrayList_init(ArrayList* array_list, int size);

// Get first element in array list and update start
int getElement(ArrayList* array_list);

// Add an element in array list and update start
void addElement(ArrayList* array_list, int elem);

void PrintArrayList(ArrayList* array_list);