#ifndef POSP_H

#define POSP_H

#include "textManager.h"
#include "HeaderSmv.h"
#include "STable.h"
#include "Node.h"
#include "Object.h"


/**
 * Recebe um objeto  (que representa expressão ou valor qualquer) e uma String (SINTH_BIND), e o converte para o seu
 * a String apropriada. Caso seja passado defaultValue como 1, o SINTH_BIND é o default do tipo daquele valor
 *
 * @param o o valor em objeto da linguagem
 * @param bind a string onde o SINTH_BIND será copiado para
 * @param index usado para tratar binds em indices de string
 * @param defaultValue uma flag que define se o default daquele tipo deve ser o SINTH_BIND
 *
 * @SideEffects Aletra o conteúdo em memória vindo de SINTH_BIND
 *
 * */


void copyValueBind(Object* o, char* bind,int index,int defaultValue);


/*
 * Atualiza o intervalo de tempo  -> "time: 0..3;" e
 * o init(time) ou next(time) dependendo se for I_TIME ou F_TIME
 * */

void updateTime(HeaderSmv* main , STable * writeSmvTypeTable, char* newValue, int type, int typeExrp,int minmax);

/**
 * Cria um cubo de condições da forma:
 *
 *  "cubo : evaluation;" ,
 *  ou ainda "cubo" caso essa não possua uma avaliação ainda definida o que fica como responsabilidade da função que cria o assign
 *
 *  Onde cubo pode seguir ser das seguintes formas:
 *
 *      opBind1 operation opBind2, caso possua dois operandos e uma operação
 *      operation opBind1, caso seja uma operação unária
 *
 *  Além disso, o cubo retornado pode possuir \n ou não na sua frente dependendo diretamente do parâmetro firstCond
 *
 * @param opBind1 o primeiro operando
 * @param opBind2 o segundo operando
 * @param operation a operação da condição
 * @param evaluation o resultado da avaliação no nuXmv, se for necessário criar um cubo com avaliação
 * @param firstCond define se o cubo retornado é terá um \n na frente,
 * dependendo diretamente do contexto da variável que irá receber essa condição
 *
 * @SideEffects  Aloca uma string em memória, delegando a responsabilidade do seu free
 *
 * */
char *createConditionCube(char *opBind1, char *opBind2, char *operation, char *evaluation, int firstCond);

/***/
void createExprBind(char *result, Object *o1, Object *o2, char *op);
/***/
void bindCondition(STable* scope, Object* conditionExpr);

/**
 * Formata a expressões usada na atribuição de uma variável qualquer, utilizando condicionais ou não.
 * Em casos condicionais, um "cubo" de condições é criado. E esse cubo recebe, um \n no inicio da linha,
 * caso não seja a primeira condição associada a aquela variável.
 *
 * @param ctime a diretiva do tempo corrente
 * @param changeContext uma flag que define se o contexto temporal foi mudado
 * @param directiveValueBind  string de SINTH_BIND da diretiva C_TIME vinda do interpretador
 * @param valueBind  string de SINTH_BIND do valor da expressão vinda do interpretador
 * @param firstCondition define se essa é a primeira condição aplicada ao assigin
 * @param defaultValueBind string de SINTH_BIND do valor default para aquela variável
 * @param expr a expressão recuperada pelo interpretador
 * @param scope o escopo de execução
 *
 * @SideEffects:
 *      aloca uma string de condições, devem ser liberadas após uso no specAssign.
 *      formata a string directiveValueBind
 *      formata a string valueBind
 *      formata a string defaultValueBind, em atribuições que necessitem init e next juntos ou em init's condicionais
 *
 * @Return "cubo : expressão;"
 * */

char *formatBinds(int ctime, int changeContext, char *directiveValueBind, char *valueBind, char *defaultValueBind,
                  Object *expr, STable *scope, int firstCondition, int initVar, int ignoreTemporal, int ignoreCond);

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

void createAssign(char *varName, HeaderSmv *header, STable *writeSmvTypeTable, const char *newValue, char *condition,
                  int typeExpr, char *defaultEvalCond);


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
void updateAssign(char* varName ,HeaderSmv* header, STable* writeSmvTypeTable, char* newValue, char* condition, int type ,int typeExpr, int minmax);

/**
 * Devolve uma referência a um objeto, considerando seu bind com redefinições e escopo original onde foi criado
 * @param a entrada original na tabela de simbolos da linguagem
 * @sideEffects:  Todos os criar uma copia do Object referênciado por var, e passa uma string do nome referênciado para o seu SYNTH_OBJECT
 * */
Object* refCopyOfVariable(TableEntry* var);

/**
 * Escolhe entre update/create Assign de casos init/next tratando casos de redefinição e condições
 * @param
 * @sideEffects:  Todos os colaterais de updateAssign ou createAssign
 * */
void specAssign(int varInit, int contextChange, TableEntry *var, HeaderSmv *header, STable *scope, STable *writeSmvTypeTable,
           Object *newValue, int redef, int typeExpr, int C_TIME);


/**
 * Para casos de redefinição, libera a entrada anterior dessa variável já que a mesma não será mais referênciada
 * @param a entrada com o nome da variável
 * @param a tabela de  simbolos auxiliar
 * @sideEffects:  Computa uma string para o nome referência e depois o libera da tabela de simbolos auxiliar
 * */
void letGoOldEntry(TableEntry* var, STable* auxTable);


void writeResultantHeaders(HeaderController* controller, const char* path);

#endif

