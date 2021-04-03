//
// Created by mateus on 27/03/2021.
//


#include <stdio.h>
#include <stdlib.h>
#include "Hash.h"

/*
int hash(char* str, int SIZE_ED){
    int LARGE_PRIME = 2049;
    int hash = 401;
    while (*str != '\0') {
        hash = ((hash << 4) + (int)(*str)) % SIZE_ED;
        str++;
    }
    return hash % LARGE_PRIME;
}
*/

int hash(char *str, int SIZE_ED)
{
    int hash = 5381;
    int c;
    char* strpt = str;

    while (c = *strpt++){
        hash = (( ( (hash << 5) + hash) + c )  % SIZE_ED); /* hash * 33 + c */
    }
    int final = hash % SIZE_ED;
    if(final >= SIZE_ED-1){
        fprintf(stderr,"[hash] Error in generating HASH\n");
        exit(-1);
    }
    return final;
}



