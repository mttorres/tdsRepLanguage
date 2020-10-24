#ifndef OBJECT_H

#define OBJECT_H

#include "Enum.h"

// melhor do que retornar void! criar "proxy" (guarda o tipo ! e o valor (evita ter problemas de derefernciação))
// pode permitir lazy evalution no futuro
typedef struct SYNTH_OBJECT
{
  int type;
  int OBJECT_SIZE;
  int STR;
  void ** values; // a pergunta é ... lista de valores ou valor único ?(em geral vai ser unico, pode ser também um ponteiro para uma lista)
  int changedType; // serve para auxiliar na escrita no nuXmv (e talvez em um futuro se permitir lista de valores diferentes)
  int binded;

} Object;

									// mudar depois o tipo
Object* createObject(int type, int OBJECT_SIZE, void** values);

void printObject(Object* o);

void letgoObject(Object* o);


Object* copyObject(Object* o);


#endif