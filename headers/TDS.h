#ifndef TDSV_H

#define TDSV_H

#include "Enum.h"
#include "Object.h"

#define MAX_DEPEND 150


//struct de TDS: 

typedef struct TDS
{
  char* name;
  TDS_TYPE type;
  Object* DATA_SPEC; // especificação da TDS, ou seja o PATH DO PROGRAMA
  Object** DATA_TIME; // todos os objetos de valor por tempo sintetizados para a TDS, usado para memoization
  struct TDS** linkedDependent; // tds's que dependem dessa
  int TOTAL_DEPENDENCIES_PT;
  int TOTAL_DEPENDENTS_PT; // o total de dependencias dessa tds (quando aplicavel)
  char* functionRef;  // a referência a função
  int delayed; // se ela é delayed ou não

/*
  // ainda não sei como isso vai ficar
  void * fpointer;
  

  // dados da tds (lista de tamanho = F_TIME - I_TIME), (ACEITAR DADOS DIFERENTES? OU SÓ UM PARA A TDS?)
  // se for um tipo só : usar union?  (pode ser um de cada vez mais não ao mesmo tempo)
  // senão usar VOID 
  void ** values; // delayed? passar lista "alternada?", ou usar object?
*/

  int I_INTERVAL;
  int F_INTERVAL;
  int* COMPONENT_TIMES;
  int AUX_REF; // devemos ter duas referências porque a tabela de simbolos auxiliar NÃO É USADA POR PORTS MODULE. (NA VERDADE É USADO SIM AGORA)
  int SMV_REF; // por outro lado, os HEADERS SÃO USADOS POR AMBOS!



} TDS;

// passar valores e função para construtor? (função já vai estar definida, valores só vai estar definido se a TDS for linked!)
TDS *
createTDS(char *name, TDS_TYPE type, Object *valueList, int delayed, char *functionRef, int I_INTERVAL, int F_INTERVAL);

/**
 * Linka duas TDS's.
 * @param tds a TDS que fornece a dependência
 * @param dependent a TDS que recebe a dependência
 */
void addTdsDependent(TDS* tds, TDS* dependent);

void* letGoTDS(struct TDS* tds);

#endif