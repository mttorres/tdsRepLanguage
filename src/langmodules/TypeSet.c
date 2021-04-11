//
// Created by mateus on 31/03/2021.
//

#include <stdlib.h>
#include <string.h>
#include "TypeSet.h"
#include "Hash.h"

int MULTIPLIER_SIMPLE_HASH = 1;


TypeSet* createTypeSet(int pos, int sizeString){
    TypeSet* ts = malloc(sizeof(TypeSet));
    ts->usedSize = MAX_SIMPLE*MULTIPLIER_SIMPLE_HASH;
    ts->hash_set = malloc(sizeof(char*)*ts->usedSize);
    int i;
    for (i = 0; i < ts->usedSize; i++) {
        ts->hash_set[i] = NULL;
    }
    ts->lastIndex = 0;
    ts->pos = pos;
    ts->size = sizeString;
    return ts;
}

int addElementToTypeSet(TypeSet* ts, char* newElement){
    int pos = hash(newElement,ts->usedSize);
    if(!ts->hash_set[pos]){
        char* aloc = malloc(sizeof(char)*(strlen(newElement)+1));
        strcpy(aloc,newElement);
        ts->hash_set[pos] = aloc;
        ts->lastIndex = pos > ts->lastIndex? pos : ts->lastIndex;
        return 1;
    }
    if(strcmp(newElement,ts->hash_set[pos]) != 0){
        // politica de redistribuição (podemos tentar implementar de novo)
        exit(-1);
    }
    return 0;
}
