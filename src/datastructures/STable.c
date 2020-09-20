#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../headers/STable.h"
#include "../../headers/constants.h"


const char* mappingEnumTable[] =  {
    "GLOBAL",
    "FUNC",
    "LOOP",
    "IF_BLOCK",
    "ELSE_BLOCK",
};


// entry
TableEntry* createEntry(char* name, Object* val, int methodParam, STable* parentScope) {
    
    TableEntry* newEntry = (TableEntry*) malloc(sizeof(TableEntry));
    newEntry->name = name; // note que para strings e para valores void criados (talvez seja necessário malloc)
    newEntry->val = val;   // objects sempre vão ser alocados, string devemos ter tratativa! (a não ser que essa seja alocada pelo bison?)
    newEntry->methodParam = methodParam;
    newEntry->parentScope = parentScope;
    newEntry->level = parentScope->level;
  	newEntry->order = parentScope->order;
    
    
    return newEntry;
    
    
}


void printEntry(TableEntry* e) {

	if(e) 
	{
		printf("\t %s :",e->name);
		//printf("\t %s : ( ",e->name);
		printObject(e->val);
		printf("\t ( methodParam: %d, level: %d, order: %d ) \n",e->methodParam,e->level,e->order);

	}

}



/// table

STable* createTable(SCOPE_TYPE type, STable* parent,  int level, int order) {

	STable* newtable = (STable*) malloc(sizeof(STable));
	
/*	
	int nt = sizechildren /sizeof (STable*);
	STable** chillist;
	if(nt){
	    chillist = (STable**) malloc(nt*sizeof(STable*));   
	    // quais situações? PASSAR FILHOS COM VAR_ARGS ? (parece complicar desnecessáriamente...)(tabelas não vao ser criadas já com escopos filhos!)
	    // se não for feito assim vai literalmente não ter nenhuma utilidade passar os filhos para o construtor?
	}
*/
	newtable->nchild = 0;
	newtable->type = type;
	newtable->level = level;
	newtable->order = order;

	newtable->type = type;
	newtable->level = level;
	newtable->order = order;
	newtable->lastEntryIndex = 0;
	
	if(parent){
		newtable->parent = parent;
	}

/*
	if(chillist){
		newtable->children = chillist;
	}
*/
	
	newtable->tableData = (TableEntry**) malloc(MAX_TABLE*sizeof(TableEntry*));

	return newtable;

}

void printTable(STable* t){
	if(t){
		printf("%s (%d,%d) \n",mappingEnumTable[t->type],t->level,t->order);
		if(t->lastEntryIndex != 0 && t->tableData){
			printf("|--> Entries: \n");
			int i;
			for(i=0;i<= t->lastEntryIndex; i++){
				printEntry(t->tableData[i]);
			}
		}
	}
}


void letgoEntry(TableEntry* e) {
	if(!e) {
	    return;
	}
	free(e);
}


void letgoTable(STable* t)
{
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
		for(i=0; i < MAX_TABLE; i++)
		{
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
    printf("HASH CALCULADO para (%s) É: %d \n",e->name,index);
   	t->tableData[index] = e;
    t->lastEntryIndex = index > t->lastEntryIndex ? index : t->lastEntryIndex;
    
}

TableEntry* lookup(STable* t, const char* name) {
    
    int index = hash(name);
    return t->tableData[index];
    
}



// op's novas que funcionam

// note que ! eu poderia muito bem por que os valores FOLHA dos nós da arvore são os mesmos que ENTRY_TYPE, existe ligeira redundância...
// QUE POR SINAL.... meu entry_type vai ser o meu "proxy" para valores? 
		// acho que vai ter que ter proxy... senão vai ficar dificil manipular os TIPOS
		// JÁ QUE OS TIPOS VÃO SER LITERALMENTE REALCIONADOS A DESEMPILHAR A ARVORE
		// primeiro eu vou testar usando valores literais 
void addValueCurrentScope(char* name, void* val, int methodParam,STable* current) {

	//nome da variavel,   val vai ser literalmente o valor dela (problema, e quanto for uma lista?)
	TableEntry* entry = createEntry(name,val,methodParam,current);
	insert(current,entry);
}






//void ou retorna o filho??? Retornar parece melhor, provavelmente ao adicionar um subscope eu vou querer operar sobre ele imediatamente
STable* addSubScope(STable* parent, SCOPE_TYPE type) {
	
	STable* child = createTable(type,parent,parent->level+1,parent->nchild);

	if(!parent->nchild) {
		parent->children = (STable**) malloc(parent->nchild*sizeof(STable*));
	}
	parent->children[parent->nchild] = child;
	parent->nchild++;

	return child;

}

