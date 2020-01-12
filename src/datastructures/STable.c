#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../headers/STable.h"
#include "../../headers/constants.h"


STable* createTable(SCOPETYPE type, STable* parent,  int level, int order, STable** children, int sizechildren){

	STable* newtable = (STable*) malloc(sizeof(STable));
	int nt = sizechildren/sizeof (STable*);

	newtable->nchild = nt;
	newtable->type = type;
	newtable->level = level;
	newtable->order = order;



	if(parent){
		newtable->parent = parent;
	}

	if(children){
		newtable->children = children;
	}

	return newtable;



}

void printTable(STable* t){
	if(t){
		printf("%s (%d,%d) \n",mappingEnumTable[t->type],t->level,t->order);
	}
}


void letgoTable(STable* t){
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
