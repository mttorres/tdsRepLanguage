#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../headers/Node.h"
#include "../headers/STable.h"



void* processFuncTest1(Node* node, STable* currentScope)
{
	printf("TESTE 1 DISPARADO\n");
	int* x = malloc(sizeof(int));
	*x = 1;
	printTable(currentScope);
	printNode(node);
	return x;
}


void* processFuncTest2(Node* node, STable* currentScope)
{
	printf("TESTE 2 DISPARADO\n");
	int* x = malloc(sizeof(int));
	*x = 2;
	printTable(currentScope);
	printNode(node);
	return x;
}

// declarar com const ? (me parece "nada haver")
void* (*executores[20]) (Node*, STable*) = {processFuncTest1, processFuncTest2};


int main()
{
    printf("Hello World \n");

	STable* global = createTable(GLOBAL,NULL,0,0);

	int vali = 0;
	int valc = 0;
	int valf = 3;

	void* pa[] = {&vali};

	void* pb[] = {&valf};

	Object* num0 = createObject(T_DIRECTIVE_ENTRY, 1, pa);
	Object* num3 = createObject(T_DIRECTIVE_ENTRY, 1, pb);
	TableEntry* I_TIME = createEntry("I_TIME",num0,0,global);
	TableEntry* C_TIME = createEntry("C_TIME",num0,0,global);
	TableEntry* F_TIME = createEntry("F_TIME",num3,0,global);	

	insert(global, I_TIME);
	insert(global, C_TIME);
	insert(global, F_TIME);


	Node* expr = createNode(5,0,1,"RAWNUMBERDATA",DATA_NUM,"10");

	Node* assignable = createNode(5,0,1,"ASSIGNABLE",ASSIGN_V,"v");	

	Node* otherstmt = createNode(7,2,1,"OtherStmt-ASSIGN",OTHER_ASSIGN,assignable,expr,"=");

	Node* cmd = createNode(5,1,0,"OtherStmt",CMD_OTHER,otherstmt);

	Node* root = createNode(5,1,0,"Prog",PROG,cmd);

	int CODIGO_QUE_REPRESENTA_EXECUTOR_1 = 0;

	int CODIGO_QUE_REPRESENTA_EXECUTOR_2 = 1;

	int retorno = *(int*) executores[CODIGO_QUE_REPRESENTA_EXECUTOR_2](root,global);

	letgoTable(global);
	letgoNode(root);

	printf("RETORNO : %d\n",retorno);

	return 0;
}
