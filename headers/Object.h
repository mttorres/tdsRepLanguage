#ifndef OBJECT_H

#define OBJECT_H

#include "Enum.h"

// melhor do que retornar void! criar "proxy" (guarda o tipo ! e o valor (evita ter problemas de derefernciação))
typedef struct SYNTH_OBJECT
{
  int type;
  int OBJECT_SIZE;
  void ** values; // a pergunta é ... lista de valores ou valor único ?(em geral vai ser unico, pode ser também um ponteiro para uma lista)


} Object;

									// mudar depois o tipo
Object* createObject(int type, int OBJECT_SIZE, void** values);

void printObject(Object* o);


#endif