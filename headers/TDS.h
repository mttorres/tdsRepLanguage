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
  void* DATA_SPEC; // especificação da TDS, ou seja o PATH DO PROGRAMA (referencia a função também vai ficar aqui)
  Object** DATA_TIME; // todos os objetos de valor por tempo sintetizados para a TDS, usado para memoization
  struct TDS** linkedDependency; // tds's que estão sendo observadas por essa (são dependencias)
  int TOTAL_DEPENDENCIES_PT; // total de dependencias dessa TDS (quando aplicavel)
  int delayed; // se ela é delayed ou não
  int I_INTERVAL;
  int F_INTERVAL;
  int* COMPONENT_TIMES;
  int AUX_REF; // devemos ter duas referências porque a tabela de simbolos auxiliar NÃO É USADA POR PORTS MODULE. (NA VERDADE É USADO SIM AGORA)
  int SMV_REF; // por outro lado, os HEADERS SÃO USADOS POR AMBOS!
  // watch-list não será reintroduzida
  int noValue;
  int LAST_DELAYED_ACCEPT_TIME;

  // usados pelo filter
  void* limitCondition; // provavelmente vai ter que ser um path
  int currentCondEval;
  char* currenCondBindRef;


} TDS;

// passar valores e função para construtor? (função já vai estar definida, valores só vai estar definido se a TDS for linked!)
TDS * createTDS(char *name, TDS_TYPE type, void *valueList, int delayed, int I_INTERVAL, int F_INTERVAL, void *limitCondition);

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

/**
 * Atualiza a condição de uma TDS realizando as validações corretas
 * @param tds a tds a qual o filtro é aplicado
 * @param condExpr a condição que foi avaliada no instante de tempo atual
 */
void updateLimitCondition(TDS* tds, Object* condExpr);

/**
 * Reseta uma condição de filtro para a próxima iteração da TDS.
 * @param tds a tds atual
 */
void resetLimitConditionEval(TDS* tds);

void letGoTDS(struct TDS* tds);

void prepareToPrintTDS(TDS* tds, int C_TIME);

void printTDS(TDS* tds, TDS* dependent, int C_TIME);

#endif