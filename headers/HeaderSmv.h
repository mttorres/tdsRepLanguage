#ifndef HEADER_H

#define HEADER_H


typedef struct headersmv
{
  int type;
  long modulePointer;
  long varPointer;
  long invarPointer;
  long assignPointer;
  long transPointer;
  long CURR_OFFSET;

}HeaderSmv;


HeaderSmv* createHeader(int type,long moduleP, long varP, long assignP, long transP, long invarPointer, long offset);

void printHeader(HeaderSmv* h);

void letgoHeader(HeaderSmv* h);

HeaderSmv** initHeadersStruct(int size);

void letGoHeadersStruct(HeaderSmv** hs, int size);

char* addParamModule(char* original, char* param);

void preProcessSmv(FILE* smvP, HeaderSmv** ds);

void postProcessSmv(FILE* smvP, int* ds);


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



#endif
