#pragma once

typedef struct ArrayList{
    int* list;
    int start;
    int size;
    int max_size;
} ArrayList;

// Initialise and fill an array list of size elements
void ArrayList_init(ArrayList* array_list, const int size);

// Initialise an array list of size elements and set all elements to -1
void EmptyArrayList_init(ArrayList* array_list, const int size);

// Get first element in array list and remove it from the list
int getElement(ArrayList* array_list);

// Add an element in array list
void addElement(ArrayList* array_list, const int elem);

void PrintArrayList(const ArrayList array_list);