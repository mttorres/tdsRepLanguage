#ifndef TABLE_H

#define TABLE_H

#include "Enum.h"
#include "Object.h"

#define  MAX_TABLE 950

#define MAX_SIMPLE 100

#define  MAX_CHILD 150

struct S_TABLE;

struct S_TABLE;

typedef struct E_TABLE
{
  char* name;
  Object* val; // encapsular todo objeto da linguagem nesse novo tipo, assim não temos que lidar com "void puro"
               // porque?  Ao recuperar algo da linguagem se usarmos void diretamente fica "impossível" saber o tipo retornado
               // isso obrigaria fazer duas coisas:  ou a cada valor retornado criar uma ENTRY, mesmo que só estejamos repassando (oque seria conceitualmente errado)
               // e poderia causar efeitos colaterais na TABELA DE SIMBOLOS, ou seja é melhor encapsular um valor qualquer em outro objeto!
  int methodParam;
  int order;
  int level;
  struct S_TABLE * parentScope;
} TableEntry;


typedef struct S_TABLE
{
  struct S_TABLE* parent;
  struct S_TABLE** children;
  int nchild;
  int level;
  int order;
  SCOPE_TYPE type;
  char* conditionBind;
  TableEntry** tableData;
  int lastEntryIndex;
  int backup;
  int collision;
  int notEvaluated;
  int usedSize;
  int childOfFunction;
  int indexRef;
} STable;


TableEntry* createEntry(char* name, Object* val, int methodParam, STable* parentScope);


STable *createTable(SCOPE_TYPE type, STable *parent, int level, int order, int indexRef);


STable* addSubScope(STable* parent, SCOPE_TYPE type);

/*
    Trtamento para colisões da tabela, 
    copia os ponteiros de objetos (table entry) para um vetor de ponteiros maior
    e os redistribui de acordo com a função que agora vai fazer uso do t->collision.

    Após isso tenta novamente inserir  param :e

    efeitos colaterais: muda comportamento da função hash e chama insert novamente para param :e

*/
void redistributeHashs(STable* t, TableEntry* e);

/*
  Verifica se é necessário chamar addEntryToTypeSet
  Efeitos colaterais: chama lookup para a variavel na tabela de controle (current) e lookup para o tipo na tabela* da tupla (indice, tamanho, tabela*) 
*/
int checkTypeSet(STable* current, char* name,  char* typeid);

/*
  Adiciona um tipo para o "conjunto de tipos das variáveis" para otimizar a escrita no arquivo SMV.
  Efeitos colaterais: chama addValue para a tabela* da tupla (indice, tamanho, tabela*)
*/
void addEntryToTypeSet(STable* current, char* name, char* typeid); 

void addTypeSetSmv(char *name, void **any, int object_size, STable *current);

void addValue(char *name, void **any, int any_type, int object_size, int methodParam, STable *current, int timeContext);

/***
 * Adiciona a referência a um objeto(ED) a uma variável de um dado escopo
 * @param name o nome da variável
 * @param DATA_STRUCT a estrutura de dados (VETORES E TDS'S)
 * @param methodParam  se essa referência é passada como parâmetro ou não (provavelmente útil ao dar free já que dar free e ele existe fora da uma
 * função implicaria em perda de dados)
 * @param current o escopo corrente.
 */
void addReferenceCurrentScope(char* name, Object* DATA_STRUCT, int methodParam, STable* current);

void addValueCurrentScope(char* name, Object* val, int methodParam,STable* current);

/*
  Atualiza um objeto de uma entrada da tabela de simbolos com novo tipo, novo tamanho, e elementos
  Efeitos colaterais: chama updateObject, que muda completamente o objeto anterior e elimnando os dados antigos
*/
void updateValue(char *name, void **any, int any_type, int object_size, int oIndex, int oProp, STable *current,
                 int contextChange);

void printTable(STable* t);

void letgoTable(STable *t);

int hash(char * str, STable* t);

void insert(STable* t, TableEntry* e); 

TableEntry* lookup(STable* t, char* name);

void printEntry(TableEntry* e);


/**
 * Libera as entradas da tabela de simbolos, seguindo diferentes esquemas dependendo do type.
 *      tupla-objeto-linguagem: n objetos de mesmo tipo (inteiro, timedirective, booleano, label, null, tds)
 *      tupla-init/next: pos, tamanho, pointIni, pointEnd
 *      tupla-inteiros : pos, tamanho, pointIni, pointEnd, min, max
 *      tupla-booleano:  pos, tamanho
 *      tupla-set:   pos, tamanho, SIMPLE_HASH
 *
 * @param e a entrada a ser liberada
 * */
void letgoEntry(TableEntry *e);

/**
 * Libera as entradas da tabela de simbolos, usando o nome de uma variável dessa tabela (chama o método anterior)
 * Porque desse método? Caso seja necessário "economizar entradas" antes do fim da execução, ele limpa os valores da tabela (NULL)
 * @param a tabela a liberar os dados
 * @param name o nome da variável
 * @SideEffects chama o letGoEntry original e seta o table->data[hash] = NULL
 * */
void letGoEntryByName(STable* table, char* name);

#endif
