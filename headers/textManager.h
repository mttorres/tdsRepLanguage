#ifndef TM_H

#define TM_H

char * customCat(char* dest, char* src, char toIgnore, int ignoreMode);

char *addParams(char *original, char *param, char *delim1, char *delim2);



void clearOldPortsRefs(char* oldConstraint, char* toCopyResult);

/**
 *  Genralização, escreve no meio do intervalo  intervalo início,fim de uma String qualquer.
 *  Após isso atualiza os novos intervalos para uma próxima alteração
 *
 *  @param newValue o que será escrito
 *  @param updated o ponteiro para a string que será alterada
 *  @param sizeNew o tamanho do valor a ser escrito (subString)
 *  @param pointIni o início do intervalo
 *  @param paramEnd o fim do intervalo .
 *  @param size o tamanho da string total original
 *  @param newPointInit o ponteiro do novo ponto de partida do intervalo
 *  @param newPointEnd o ponteiro do novo fim do intervalo
 *
 *  @sideEffects :  Atualiza o updated, seja lá de onde que essa string veio em primeiro lugar
 *  (a responsabilidade de free ou realloc fica fora dessa função)
 * */
void updateSubStringInterval(const char *newValue,  char *updated, int sizeNew, int pointIni, int pointEnd, int size, int *newPointInit, int *newPointEnd);


#endif
