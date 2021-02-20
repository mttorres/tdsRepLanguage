#ifndef TDSV_H

#define TDSV_H

#include "Enum.h"
#include "Object.h"


//struct de TDS: 

typedef struct TDS
{
  char* name;
  TDS_TYPE type;
  Object* DATA_TIME;
  Object* linked;
  char* functionRef;
  int delayed;

  // ainda não sei como isso vai ficar
  void * fpointer;
  

  // dados da tds (lista de tamanho = F_TIME - I_TIME), (ACEITAR DADOS DIFERENTES? OU SÓ UM PARA A TDS?)
  // se for um tipo só : usar union?  (pode ser um de cada vez mais não ao mesmo tempo)
  // senão usar VOID 
  void ** values; // delayed? passar lista "alternada?", ou usar object?

  int I_INTERVAL;
  int F_INTERVAL;
  int* COMPONENT_TIMES;
  int AUX_REF; // devemos ter duas referências porque a tabela de simbolos auxiliar NÃO É USADA POR PORTS MODULE.
  int SMV_REF; // por outro lado, os HEADERS SÃO USADOS POR AMBOS!



} TDS;

// passar valores e função para construtor? (função já vai estar definida, valores só vai estar definido se a TDS for linked!)
TDS *createTDS(char *name, TDS_TYPE type, Object *valueList, Object *linkedTDSs, int delayed, char *functionRef,
               int I_INTERVAL, int F_INTERVAL);

void* letGoTDS(struct TDS* tds);

#endif