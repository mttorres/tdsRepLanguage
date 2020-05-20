#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define MAX_TABLE 550

const char* mappingEnumTable[] =  {
    "GLOBAL",
    "FUNC",
    "LOOP",
    "IF_BLOC",
    "ELSE_BLOCK",
};

const char* mappingEnumEntry[] =  {
    "VAR",
    "TDS",
    "T_DIRECTIVE",
};

typedef enum { GLOBAL, FUNC, LOOP, IF_BLOCK, ELSE_BLOCK} SCOPE_TYPE;

typedef enum { VAR, TDS, T_DIRECTIVE} ENTRY_TYPE;

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

TableEntry* createEntry(char* name, int type, char* val, int methodParam, STable* parentScope) {
    
    TableEntry* newEntry = (TableEntry*) malloc(sizeof(TableEntry));
    newEntry->name = name;
    newEntry->type = type;
    newEntry->val = val;
    newEntry->methodParam = methodParam;
    newEntry->parentScope = parentScope;
    
    
    return newEntry;
    
    
}

void printEntry(TableEntry* e) {
    printf("%s : (%s, %s, methodParam: %d ) \n",e->name,mappingEnumEntry[e->type],e->val,e->methodParam);
    if(e->parentScope){
        printTable(e->parentScope);
    }
    
}

STable* createTable(int type, STable* parent,  int level, int order, STable** children, int sizechildren){

	STable* newtable = (STable*) malloc(sizeof(STable));
	int nt = sizechildren /sizeof (STable*);
	STable** chillist;
	if(nt){
	    chillist = (STable**) malloc(nt*sizeof(STable*));   
	    // quais situações? PASSAR FILHOS COM VAR_ARGS ? (parece complicar desnecessáriamente...)
	    // se não for feito assim vai literalmente não ter nenhuma utilidade passar os filhos para o construtor?
	}
	newtable->nchild = nt;
	newtable->type = type;
	newtable->level = level;
	newtable->order = order;
	
	if(parent){
		newtable->parent = parent;
	}

	if(chillist){
		newtable->children = chillist;
	}
	
	newtable->tableData = (TableEntry**) malloc(MAX_TABLE*sizeof(TableEntry*));

	return newtable;



}

void printTable(STable* t){
	if(t){
		printf("%s (%d,%d) \n",mappingEnumTable[t->type],t->level,t->order);
	}
}

void letgoEntry(TableEntry* e){
	if(!e) {
	    return;
	}
	free(e);
}

void letgoTable(STable* t){
	if(!t) {
	    return;
	}
	int i;
	if(t->children){
		for(i=0; i < t->nchild; i++){
			letgoTable(t->children[i]);
		}
		free(t->children);
	}
	if(t->tableData){
		for(i=0; i < MAX_TABLE; i++){
			if(t->tableData[i]) {
			 letgoEntry(t->tableData[i]);   
			}
		}
	    free(t->tableData);
	}
	free(t);
}

int hash(const char * str) {
	int hash = 401;
	int c;

	while (*str != '\0') {
		hash = ((hash << 4) + (int)(*str)) % MAX_TABLE;
		str++;
	}

	return hash % MAX_TABLE;
}

void insert(STable* t, TableEntry* e) {
    
    int index = hash(e->name);
    printf("HASH CALCULADO É: %d \n",index);
    t->tableData[index] = e;
    
    
}

TableEntry* lookup(STable* t, const char* name) {
    
    int index = hash(name);
    return t->tableData[index];
    
}


int main()
{
    printf("Hello World \n");
    STable* nova = createTable(GLOBAL,NULL,0,0,NULL,0);
	printTable(nova);
	TableEntry* entrada1 = createEntry("tdsA",TDS,"bigfuckingstruct",1,nova);
	TableEntry* entrada2 = createEntry("tdsB",TDS,"pequenastruct(list)",0,nova);
	
	insert(nova, entrada1);
	insert(nova, entrada2);
	
	
	printEntry(lookup(nova, "tdsA"));
	printEntry(lookup(nova, "tdsB"));
	
	
	
	
	letgoTable(nova);
    return 0;
}
