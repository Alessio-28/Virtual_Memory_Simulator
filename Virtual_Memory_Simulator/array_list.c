#include "array_list.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void ArrayList_init(ArrayList* array_list, const int size){
    assert(array_list->list && "Array list pointer must not be NULL");

    array_list->start = 0;
    array_list->size = size;
    array_list->max_size = size;

    for(int i = 0; i < size-1; ++i)
        array_list->list[i] = i+1;

    array_list->list[size-1] = -1;
    
}

void EmptyArrayList_init(ArrayList* array_list, const int size){
    assert(array_list->list && "Array list pointer must not be NULL");
    
    array_list->start = -1;
    array_list->size = 0;
    array_list->max_size = size;

    for(int i = 0; i < size; ++i)
        array_list->list[i] = -1;
}

int getElement(ArrayList* array_list){
    if(array_list->size == 0) return -1;

    const int elem = array_list->start;
    array_list->start = array_list->list[elem];
    array_list->list[elem] = -1;
    (array_list->size)--;

    return elem;
}

void addElement(ArrayList* array_list, const int elem){
    assert((elem >= 0) && "Array list: element must be non negative");
    assert((elem < array_list->max_size) && "Array list: element must be less than max size");
    assert((array_list->size < array_list->max_size) && "Array list full");
    assert((array_list->list[elem] == -1) && "Array list: element already in the list");

    array_list->list[elem] = array_list->start;
    array_list->start = elem;
    (array_list->size)++;
}

void PrintArrayList(const ArrayList array_list){
    const char* str = "Array list max size _ %d\n"
                      "Array list size _____ %d\n"
                      "Array list start ____ %d\n"
                      "Array list:";
    printf(str, array_list.max_size, array_list.size, array_list.start);
    for(int i = 0; i < array_list.max_size; ++i)
        printf(" %d", array_list.list[i]);
    printf("\n");
}