// Created by mateus on 31/03/2021.
//

#ifndef TDSREPLANGUAGE_TYPESET_H
#define TDSREPLANGUAGE_TYPESET_H

#include "Hash.h"


typedef struct TypeSet
{
    //int pos;
    //int size;
    char** hash_set;
    int usedSize;
    int lastIndex;

} TypeSet;

TypeSet* createTypeSet(char* defaultWord);

int addElementToTypeSet(TypeSet* ts, char* newElement);

// não existem operaçõs intermediaras com variáveis que vão retornar o type-set. O que vai acontecer é que
// não "atualizaremos" ele durante o eval expr, mas no final do add type-set ou ainda do update type-set
// é que compararemos o type-set sintetizado com o que está na tabela auxiliar,
//
// na criação poderemos utilizar o type-set que foi sintetizado ao invés de criar
//  -> caso a string não exista no controller, alocar nele e depois passar ref para o type-set novo
//
// na atualização (método merge) se eles tiverem divergencia, atualizaremos
// o da tabela e daremos free no outro.
//  -> o merge atualiza uma ref (vinda do env) para o type-set que ficou na tabela e da free no outro que foi sintetizado

/**
 * Cria um novo objeto type-set que contem referência as trings todos os objetos do type-set anterior e os demais campos alocados
 * @param original o type-set original
 * @return uma "copia" do type-set original
 */
TypeSet* copyTypeSet(TypeSet* original);

/**
 * Libera o type-set, de forma a dar free em toda a memoria utilizada por ele (menos as strings que vem do dict, são responsabilidades
 * do controller.
 * @param ts o type-set a ser liberado
 */
void letGoTypeSet(TypeSet* ts);

#endif //TDSREPLANGUAGE_TYPESET_H