#pragma once

typedef struct ArrayList{
    int* list;
    int start;
} ArrayList;

// Initialise array list, filling it from first (included) to last (excluded)
void ArrayList_init(ArrayList* list, int first, int last, int size);

// Get first element in array list and update start
int getElement(ArrayList* list);

// Add an element in array list and update start
void addElement(ArrayList* list, int elem);