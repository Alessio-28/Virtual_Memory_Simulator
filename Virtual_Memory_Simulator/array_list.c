#include "array_list.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void ArrayList_init(ArrayList* array_list, int first, int last, int size){
    assert(array_list->list && "Array list pointer must not be NULL");
    assert((first <= last) && "Array list: first must be less than last");
    assert((last < size) && "Array list: last must be less than size");

    array_list->start = first;

    int i = 0;
    for(; i < first; ++i)
        array_list->list[i] = -1;

    for(i = first; i < last; ++i)
        array_list->list[i] = i+1;

    for(i = last; i < size; ++i)
        array_list->list[i] = -1;

    array_list->size = last - first + 1;    
    array_list->max_size = size;
}

void EmptyArrayList_init(ArrayList* array_list, int size){
    assert(array_list->list && "Array list pointer must not be NULL");
    
    array_list->start = -1;
    array_list->size = 0;
    array_list->max_size = size;

    for(int i = 0; i < size; ++i)
        array_list->list[i] = -1;
}

int getElement(ArrayList* array_list){
    if(array_list->size == 0) return -1;

    int elem = array_list->start;
    array_list->start = array_list->list[elem];
    array_list->list[elem] = -1;
    (array_list->size)--;

    return elem;
}

void addElement(ArrayList* array_list, int elem){
    assert((elem >= 0) && "Array list: element must be non negative");

    if(array_list->size == array_list->max_size){
        printf("Array list full\n");
        exit(EXIT_FAILURE);
    }

    if(array_list->list[elem] != -1){
        printf("Array list: element already in the list\n");
        exit(EXIT_FAILURE);
    }

    array_list->list[elem] = array_list->start;
    array_list->start = elem;
    (array_list->size)++;
}

void PrintArrayList(ArrayList* array_list){
    printf("Array list start: %d\n", array_list->start);
    printf("Array list:");
    for(int i = 0; i < array_list->max_size; ++i)
        printf(" %d", array_list->list[i]);
    printf("\n");
    printf("Array list size: %d\n", array_list->size);
    printf("Array list max size: %d\n", array_list->max_size);
}
