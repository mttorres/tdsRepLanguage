#define DARRAY_H
#include "../../headers/Node.h"

typedef struct DArray
{
    Node** array;
    size_t used;
    size_t size;

}DArray;


void initArray(DArray *a, size_t initialSize);

void insertArray(DArray *a, Node* element);

void freeArray(Array *a); 




