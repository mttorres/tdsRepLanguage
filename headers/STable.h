#ifndef TABLE_H

#define TABLE_H

#include "Enum.h"
#include "Object.h"

#define  MAX_TABLE 950

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
  TableEntry** tableData;
  int lastEntryIndex;
    
} STable;


TableEntry* createEntry(char* name, Object* val, int methodParam, STable* parentScope);


STable* createTable(SCOPE_TYPE type, STable* parent,  int level, int order);


STable* addSubScope(STable* parent, SCOPE_TYPE type);



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

void addTypeSet(char* name, void** any, int any_type, int object_size, STable* current);

void addValue(char* name, void** any, int any_type, int object_size ,int methodParam, STable* current); 

void addValueCurrentScope(char* name, Object* val, int methodParam,STable* current);

void printTable(STable* t);

void letgoTable(STable* t);

int hash(const char * str);

void insert(STable* t, TableEntry* e); 

TableEntry* lookup(STable* t, const char* name);

void printEntry(TableEntry* e);

void letgoEntry(TableEntry* e);




#endif
