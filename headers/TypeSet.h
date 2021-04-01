//
// Created by mateus on 31/03/2021.
//

#ifndef TDSREPLANGUAGE_TYPESET_H
#define TDSREPLANGUAGE_TYPESET_H


typedef struct TypeSet
{
    int pos;
    int size;
    char** hash_set;
    int usedSize;
    int lastIndex;
} TypeSet;

TypeSet* createTypeSet(int pos, int sizeString);

int addElementToTypeSet(TypeSet* ts, char* newElement);

#endif //TDSREPLANGUAGE_TYPESET_H
