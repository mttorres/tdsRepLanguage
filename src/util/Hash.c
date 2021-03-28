//
// Created by mateus on 27/03/2021.
//

#include "Hash.h"

int hash(char* str, int SIZE_ED){
    int hash = 401;
    while (*str != '\0') {
        hash = ((hash << 4) + (int)(*str)) % SIZE_ED;
        str++;
    }
    return hash % SIZE_ED;
}

