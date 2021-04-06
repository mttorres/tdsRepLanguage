//
// Created by mateus on 31/03/2021.
//

#include <stdlib.h>
#include <string.h>
#include "../headers/TypeSet.h"

int MULTIPLIER_SIMPLE_HASH = 1;


TypeSet* createTypeSet(char* defaultWord){
    TypeSet* ts = malloc(sizeof(TypeSet));
    ts->usedSize = MAX_SIMPLE*MULTIPLIER_SIMPLE_HASH;
    ts->hash_set = malloc(sizeof(char*)*ts->usedSize);
    int i;
    for (i = 0; i < ts->usedSize; i++) {
        ts->hash_set[i] = NULL;
    }
    if(defaultWord){
        int hashDefaultWord = hash(defaultWord,ts->usedSize);
        ts->hash_set[hashDefaultWord] = defaultWord;
    }
    ts->lastIndex = 0;
    return ts;
}

TypeSet* copyTypeSet(TypeSet* original){
    if(original){
        TypeSet* newCopy = createTypeSet(NULL);
        int i;
        int tempIndex = 0;
        for (i = 0; i < original->lastIndex; i++) {
            if(original->hash_set[i]){
                addElementToTypeSet(newCopy,original->hash_set[i]);
                tempIndex = i;
            }
        }
        newCopy->lastIndex = tempIndex;
        return newCopy;
    }
    return NULL;
}

int addElementToTypeSet(TypeSet* ts, char* newElement){
    int pos = hash(newElement,ts->usedSize);
    if(!ts->hash_set[pos]){
        //char* aloc = malloc(sizeof(char)*(strlen(newElement)+1)); // vai pegar do dicionário! lembre-se disso
        //strcpy(aloc,newElement);
        //ts->hash_set[pos] = aloc;
        ts->hash_set[pos] = newElement;
        ts->lastIndex = pos > ts->lastIndex? pos : ts->lastIndex;
        return 1;
    }
    if(strcmp(newElement,ts->hash_set[pos]) != 0){
        // politica de redistribuição (podemos tentar implementar de novo)
        exit(-1);
    }
    return 0;
}

void letGoTypeSet(TypeSet* ts){
    if(ts){
        int i;
        for (i = 0; i < ts->usedSize; i++) {
            ts->hash_set[i] = NULL;
        }
        free(ts->hash_set);
        free(ts);
    }
}
