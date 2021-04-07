//
// Created by mateus on 31/03/2021.
//

#include <stdlib.h>
#include <string.h>
#include "../headers/TypeSet.h"

//int MULTIPLIER_SIMPLE_HASH = 1;


TypeSet* createTypeSet(char* defaultWord){
    TypeSet* ts = malloc(sizeof(TypeSet));
    ts->usedSize = MAX_SIMPLE*1;
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

/*Será atualizado  após descobrir o memory leak*/
/**
 * Dado uma variável de interesse de um módulo (ex: value (tds) return (funções de tds). Atualiza
 * o type-set dessa variável baseado no tipo SMV de uma outra variável de dependência (min max ou outro type-set).
 * @param var o nome da variável de interesse
 * @param headerModule o header do módulo em que a alteração será feita
 * @param auxTableModule a tabela de simbolos auxiliar desse módulo
 * @param varD o nome (referência) da variável dependência
 * @param auxTableDependency  a tabela auxiliar que tem as inforamções do tipo da dependência
 * @SideEffects: Realoca a string da declaração de var para comportar cada valor possível do tipo da dependência
 */
//void createUnionAtSmvType(char* var, HeaderSmv* headerModule, STable* auxTableModule, char* varD, STable* auxTableDependency);

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