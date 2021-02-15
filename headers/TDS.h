#ifndef TDSV_H

#define TDSV_H

#include "Enum.h"
#include "Object.h"


//struct de TDS: 

typedef struct TDS
{
  char* name;
  int type;
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



} TDS;

// passar valores e função para construtor? (função já vai estar definida, valores só vai estar definido se a TDS for linked!)
TDS* createTDS(char* name, int type, int I_TIME, int F_TIME, int C_TIME, int linked, int delayed);

#endif