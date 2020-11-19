#ifndef POSP_H

#define POSP_H

#include "textManager.h"
#include "HeaderSmv.h"
#include "STable.h"
#include "Node.h"
#include "Object.h"

/*
 * Atualiza o intervalo de tempo  -> "time: 0..3;" e
 * o init(time) ou next(time) dependendo se for I_TIME ou F_TIME
 * */

void updateTime(HeaderSmv* main , STable * writeSmvTypeTable, const char* newValue, int type, int typeExrp,int minmax);


char *createConditionCube(char *opBind1, char *opBind2, char *operation, char *evaluation);

/**
 *  Cria uma declaração do tipo init(varName) := newValue ; ou  init(varName) := case condition : newValue esac;
 *  Onde varName pode ser varName ou redef{redef}VarName.
 *
 *  @param varName o nome da nova variável
 *  @param condition a o cubo de condições associadas, caso exista.
 *  @param header a estrutura auxiliar associada com a localidade (main ou ports module) para a escrita em arquivo .smv
 *  @param writeSmvTypeTable a tabela de simbolos auxiliar para escrita em arquivo .smv
 *  @param newValue o valor que irá substituir ou ser adicionado a uma atribuição simples ou condicional
 *  @param condition o cubo de condições associadas, caso exista.
 *  @param redef se a variável é uma redefinição (é o numero de redefinições)
 *  @param typeExpr o tipo se é init ou next
 *  @param type o tipo da nova variável
 *
 *  @sideEffects: Adiciona uma nova string ao buffer do header->assignBuffer[posNova], e consequnetemente atualiza o POS desse header.
 *  além disso, cria uma na entrada da tabela de simbolos desse init e cria uma associada ao varName com o tipo (typeSet) associado.
 * */

void createAssign(char* varName ,HeaderSmv* header, STable* writeSmvTypeTable, const char* newValue, const char* condition, int redef, int typeExpr ,int type);


/**
 *  Atualiza uma String do tipo init/next(varName) = ... ;
 *  É usado para atualizar as atribuições de diretivas temporais, e atribuições case.
 *
 *  @param varName o nome da variável
 *  @param header a estrutura auxiliar associada com a localidade (main ou ports module) para a escrita em arquivo .smv
 *  @param writeSmvTypeTable a tabela de simbolos auxiliar para escrita em arquivo .smv
 *  @param newValue o valor que irá substituir ou ser adicionado a uma atribuição simples ou condicional
 *  @param condition a o cubo de condições associadas, caso exista.
 *  @param type o tipo que será usado na tabela de simbolos typeSet
 *  @param typeExpr o tipo se é init ou next
 *
 *  @sideEffects :  Atualiza o header->assignBuffer[pos] recuperado da tabela de simbolos writeSmvTYpeTable,
 *  além disso, atualiza na entrada da tabela da simbolos o tipo (typeSet) associada a varName,
 *  se o tipo mudar (caso de next após mudança de contexto).
 * */
void updateAssign(char* varName ,HeaderSmv* header, STable* writeSmvTypeTable, const char* newValue, const char* condition, int type ,int typeExpr, int minmax);


#endif

