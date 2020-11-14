#ifndef TM_H

#define TM_H

char * customCat(char* dest, char* src, char toIgnore, int ignoreMode);

char *addParams(char *original, char *param, char *delim1, char *delim2);


// o que vem antes tem que ser concatenado atrás!
//  --> alocar string de tamanho = dif e concatenar mais após isso 
//      --> problema: alocar = dif requer percorrer o(dif), ou seja, se eu quiser ter a string ATÉ O DELIMITADOR (customcat)
//      e saber o tamanho eu teria que realizar 2 operações uma de contar e copiar a string!
//      melhor seria: tentar fazer AO MESMO TEMPO, mas isso não parece possível... 
//      ao mesmo tempo eu já teria que ter a string de destino alocada!
//      solução possível? Alocar string pelo menos TÃO GRANDE QUANTO A ORIGINAL e a cada passada "liberar" memoria de volta 
//      ou ao fim do processo dar realloc na string
//  --> criar "proximo dif" e alocar mais e concatenar 
//  repetir até o fim da string
char* clearOldPortsRefs(char* oldConstraint);

/**
 *  Genralização, escreve no meio do intervalo  intervalo início,fim de uma String qualquer.
 *  Após isso atualiza os novos intervalos para uma próxima alteração
 *
 *  @param newValue o que será escrito
 *  @param updated o ponteiro para a string que será alterada
 *  @param sizeNew o tamanho do valor a ser escrito (subString)
 *  @param pointIni o início do intervalo
 *  @param paramEnd o fim do intervalo .
 *  @param size o ponteiro do novo tamanho cálculado
 *  @param newPointInit o ponteiro do novo ponto de partida do intervalo
 *  @param newPointEnd o ponteiro do novo fim do intervalo
 *
 *  @sideEffects :  Atualiza o updated, seja lá de onde que essa string veio em primeiro lugar
 *  (a responsabilidade de free ou realloc fica fora dessa função)
 * */
void updateSubStringInterval(const char *newValue,  char *updated, int sizeNew, int pointIni, int pointEnd, int *size, int *newPointInit, int *newPointEnd);


#endif
