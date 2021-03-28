#ifndef OBJECT_H

#define OBJECT_H

#include "Enum.h"

// melhor do que retornar void! criar "proxy" (guarda o tipo ! e o valor (evita ter problemas de derefernciação))
// pode permitir lazy evalution no futuro
typedef struct SYNTH_OBJECT
{
    object_type type;
    int OBJECT_SIZE;
    int STR; // tamanho strings mapeado
    void ** values; // a pergunta é ... lista de valores ou valor único ?(em geral vai ser unico, pode ser também um ponteiro para uma lista)
    int redef; // serve para auxiliar na escrita no nuXmv (e talvez em um futuro se permitir lista de valores diferentes)
    char* SINTH_BIND; // auxilia a variavel anterior, // serve para auxiliar a limpeza de memória e escrever no nuXmv
    int timeContext;
    int aList;

} Object;


/**
 * Aloca um objeto atômicos da linguagem (que não sejam uma estrutura TDS ou TimeComponent)
 * @param type o tipo do objeto
 * @param OBJECT_SIZE o tamanho do objeto
 * @param values os valores passados
 * @param timeContext o contexto temporal
 * @param BIND o bind sintetizado para aquele objeto
 * @return um objeto qualquer da linguagem que não envolva estruturas de dados
 * @SideEffects: Aloca uma lista de ponteiros para void de tamanho OBJECT_SIZE e para parte da lista aloca um
 * ponteiro para o tipo apropriado. Ver métodos: allocatePtObjects e allocateTypeSetObjects
 */
Object *createObject(object_type type, int OBJECT_SIZE, void **values, int timeContext, char *BIND);

/**
 * Aloca um Object em forma de estrutura dados composta de objetos
 * @param type o tipo da lista (e de todos os seus objetos)
 * @param OBJECT_SIZE o tamanho da lista
 * @param value os valores a serem passados
 * @param aList flag para tratamento de alocação de lista
 * @return Aloca um object composto de outros objects já alocados anteriormente.
 */
Object* createObjectDS(object_type type, int OBJECT_SIZE, void ** values, int timeContext, char *BIND, int aList);

void printObject(Object* o);

void letgoObject(Object *o);

/*
	Copia um objeto "o"
*/
Object* copyObject(Object* o);

/*
	Atualiza as definições de um objeto "o"
*/
void updateObject(Object *o, void **any, int any_type, int object_size, int index, int prop, int contextChange);

/***
 * Junta duas componentes sintetizadas de uma lista em uma
 * nova lista (Objeto do tipo GEN_LIST ou do mesmo tipo que os demais membros da lista (para os objetos não internos da linguagem) ).
 * É usado para parâmetros, listas, e outras estruturas recursivas da AST.
 *
 * @param LEFT_COMPONENT o objeto mais a esquerda LISTA ou valor único
 * @param RIGHT_COMPONENT o objeto mais a direita, um valor únco
 * @param separator um separador utilizado para sintetizar expressões no SMV (, &, |) ... (talvez nem seja usado)
 * @return uma lista genérica ou uma lista de objetos comuns da linguagem
 */
Object * mergeGenericList(Object* LEFT_COMPONENT, Object* RIGHT_COMPONENT);

/**
 * Atualiza um objeto que era NULL
 * @param var o objeto original
 * @param expr o novo objeto
 * @SideEffects: Aloca um BIND para var, aloca um values para var caso expr não seja uma lista, já que irá ocorrer copia
 * de valores.
 */
void updateNullRef(Object *var, Object *expr);

#endif