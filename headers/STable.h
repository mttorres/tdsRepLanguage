#ifndef TABLE_H

#define TABLE_H




#define  MAX_TABLE 550;

struct S_TABLE;

typedef struct E_TABLE
{
  char* name;
  int type;
  char* val;
  int methodParam;
  struct S_TABLE * parentScope;
} TableEntry;


typedef struct S_TABLE
{
  struct S_TABLE* parent;
  struct S_TABLE** children;
  int nchild;
  int level;
  int order;
  int type;
  // TABLE DATA (como representar!?)(ponteiros pra void?) (ou criar outra struct chamada TABLE ENTRY)
  TableEntry** tableData;
    
} STable;


TableEntry* createEntry(char* name, int type, char* val, int methodParam, STable* parentScope);


STable* createTable(int type, STable* parent,  int level, int order, STable** children, int sizechildren);

void printTable(STable* t);

void letgoTable(STable* t);

int hash(const char * str);

void insert(STable* t, TableEntry* e); 

TableEntry* lookup(STable* t, const char* name);


void printEntry(TableEntry* e);

void letgoEntry(TableEntry* e);




#endif
