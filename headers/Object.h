#ifndef OBJECT_H

#define OBJECT_H

#include "Enum.h"

// melhor do que retornar void! criar "proxy" (guarda o tipo ! e o valor (evita ter problemas de derefernciação))
// pode permitir lazy evalution no futuro
typedef struct SYNTH_OBJECT
{
  int type;
  int OBJECT_SIZE;
  int* STR; // tamanho strings mapeado
  void ** values; // a pergunta é ... lista de valores ou valor único ?(em geral vai ser unico, pode ser também um ponteiro para uma lista)
  int redef; // serve para auxiliar na escrita no nuXmv (e talvez em um futuro se permitir lista de valores diferentes)
  char* bind; // serve para auxiliar a limpeza de memória e escrever no nuXmv
  int timeContext;

} Object;

									// mudar depois o tipo
									Object *createObject(int type, int OBJECT_SIZE, void **values, int timeContext);

void printObject(Object* o);

void letgoObject(Object* o, int always);

/*
	Copia um objeto "o"
*/
Object* copyObject(Object* o);

/*
	Atualiza as definições de um objeto "o"
*/
void updateObject(Object *o, void **any, int any_type, int object_size, int index, int prop, int contextChange);

#endif