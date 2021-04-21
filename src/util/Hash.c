//
// Created by mateus on 27/03/2021.
//

#include "../../headers/Hash.h"
#include <stdio.h>
#include <stdlib.h>

/**
 *
 * djb2 * This algorithm was first reported by Dan Bernstein
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
    if(final > SIZE_ED-1){
        fprintf(stderr,"[hash] Error in generating HASH\n");
        exit(-1);
    }
    return final;
}

