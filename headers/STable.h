//#ifdef TABLE_H

#define TABLE_H


#define  MAX_TABLE 550;



typedef enum { GLOBAL, FUNC, LOOP, IF, ELSE} SCOPETYPE;


typedef struct S_TABLE
{
  struct S_TABLE* parent;
  struct S_TABLE** children;
  int nchild;
  int level;
  int order;
  SCOPETYPE type;
  // TABLE DATA (como representar!?)(ponteiros pra void?) (ou criar outra struct chamada TABLE ENTRY)
} STable;


STable* createTable(SCOPETYPE type, STable* parent,  int level, int order, STable** children, int sizechildren);

void printTable(STable* t);

void letgoTable(STable* t);

int hash(const char * str); 

//#endif