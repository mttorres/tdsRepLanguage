#ifndef TDSV_H

#define TDSV_H

#include "Enum.h"
#include "Object.h"
#include "Hash.h"

#define MAX_DEPEND 150


//struct de TDS: 

typedef struct TDS
{
  char* name;
  TDS_TYPE type;
  Object* DATA_SPEC; // especificação da TDS, ou seja o PATH DO PROGRAMA
  Object** DATA_TIME; // todos os objetos de valor por tempo sintetizados para a TDS, usado para memoization
  struct TDS** linkedDependency; // tds's que estão sendo observadas por essa (são dependencias)
  int TOTAL_DEPENDENCIES_PT; // total de dependencias dessa TDS (quando aplicavel)
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
  // watch-list não será reintroduzida
  int noValue;
  Object* limitCondition; // provavelmente vai ter que ser um path
  int LAST_DELAYED_ACCEPT_TIME;


} TDS;

// passar valores e função para construtor? (função já vai estar definida, valores só vai estar definido se a TDS for linked!)
TDS * createTDS(char *name, TDS_TYPE type, Object *valueList, int delayed, char *functionRef, int I_INTERVAL, int F_INTERVAL, Object *limitCondition);

/**
 * Linka duas TDS's.
 * @param tds a TDS que fornece a dependência
 * @param dependent a TDS que recebe a dependência
 */
void addTdsDependency(TDS* dependency, TDS* tds);

/**
 * Adiciona um dado sintetizado para uma TDS indexado por instantes de tempo.
 * Caso o instante não possua dado, realiza a operação e retorna 1,
 * caso contrário retorna zero.
 * @param currentTDS a tds a ser modificada
 * @param C_TIME o tempo corrente
 * @param value o valor
 * @return 1 caso a operação tenha sido realizada, 0 caso contrário
 */
int addDataToTds(TDS* currentTDS, int C_TIME, Object* value);

/**
 * Dada uma TDS, resolve todas as dependências de valor que ela possui em um determinado instante de tempo.
 * Caso mais de uma resolução seja encontrada, não faz nada. (MERGER)
 * @param tds a tds atual
 * @param C_TIME o tempo corrente
 */
void resolveMergerTdsDependencies(TDS *tds, int C_TIME);

/**
 * Resolve a dependência de uma TDS que segue o padrão delayed.
 * Usa o tempo corrente para determinar se estamos em um instante futuro ao I_INTERVAL
 * delayed só aceita 1 input, e esse deve seguir a seguinte regra:
 *
 *	init(d.value):= NULL;
 *	    next(d.value):= case
 *				d.value = NULL : c.value;
 *				c.value = NULL & d.value != NULL : NULL;
 *				TRUE : d.value;
 *         esac;
 * Dessa forma referenciando um valor anterior da TDS delayed e tendo um ponteiro próprio para esse
 * de forma que quando um valor não é aceito pela porta ele se mantem no anterior.
 *
 * @param tds a tds delayed
 * @param C_TIME o tempo de referência
 */
void resolveDelayedTdsDependencies(TDS* tds, int C_TIME);

/**
 * Devolve o valor de uma TDS em um determinado instante de tempo. A não ser que essa seja delayed,
 * ai ele referencia o último valor "aceito".
 * @param tds a tds a qual necessitamos do valor
 * @param C_TIME o tempo corrente
 * @return uma copia do valor da TDS no instante corrente
 * @SideEffects: Aloca um novo object que deve ser liberado pelo chamador depois
 */
Object* getTdsValue(TDS* tds, int C_TIME);

void* letGoTDS(struct TDS* tds);

#endif