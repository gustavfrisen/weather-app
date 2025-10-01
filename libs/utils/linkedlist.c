#include "linkedlist.h"

#include <stdlib.h>

int LinkedList_Initialize(LinkedList* _List)
{
    _List->length = 0;
    _List->head = NULL;
    _List->tail = NULL;

    return 0;
}

