#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../headers/PostProcess.h"


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


Object* evalNUM(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
{

	//int* sint = (int*) malloc(sizeof(int));
	//int* sint= atoi(n->leafs[0]);

	int* sint;
	*sint = atoi(n->leafs[0]);


	printf("[evalNUM] SINTH: %d \n",*sint);
	
	void* ip[] = {sint};

	Object* o = createObject(NUMBER_ENTRY, 1, ip);


	return o;
}

Object* evalBOOL(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
{

	int* sint;

	char* trueString = "true";

	*sint= strstr(n->leafs[0],trueString)? 1 : 0;

	printf("[evalBOOL] SINTH: %d \n",*sint);
	
	void* bp[] = {sint};

	Object* o = createObject(LOGICAL_ENTRY, 1, bp);

	return o;
}


Object* evalSTRING(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
{

	char* sint =  n->leafs[0];
	
	void* sp[] = {sint};

	printf("[evalSTRING] SINTH: %s \n",sint);
	
	Object* o = createObject(LABEL_ENTRY, 1, sp);

	return o;
}


/*
	Pensar depois... encapsular NULL ?

*/


Object* evalNULL(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
{

	// se eu interpretar como "NULL" do C mesmo podemos ter problemas(?)
	char* sint =  n->leafs[0];
	
	void* np[] = {sint};

	printf("[evalNULL] SINTH: %s \n",sint);

	Object* o = createObject(NULL_ENTRY, 1, np);

	return o;
}


Object* evalIDVAR(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
{

	// VAI RECUPERAR UM OBJETO NA TABELA DE SIMBOLOS e então SUBIR COM ELE  
	//(já que o tipo pode ser qualquer e a gente vai decidir o que fazer com ele ainda)


	TableEntry* entry = lookup(scope,n->leafs[0]);

	if(!entry)
	{
		fprintf(stderr, "%s NOT DECLARED!", n->leafs[0]);
		exit(-1);
	}
	else
	{
		// não pode retornar O MESMO PONTEIRO OU O OBJECT, senão teremos efeitos colaterais (ao mexer nesse mesmo usado por outra variavel alterariamos a anterior)
		//entry->val

		// DEVE-SE TER UMA COPIA DO OBJETO INTEIRO JÁ QUE NÃO É SIMPLESMENTE ALGO ATOMICO, tipo um número que depois vai ser jogado para cima e terá passos intermediarios
		// é um COPIA DE VARIAVEL , 

		// ou em casos de ED, uma referencia a uma ED já existente (a ideia é fazer a mesma coisa com TDS ao invés de copiar por razões óbvias)
		
		// precisamos alocar um vetor de ponteiros para void ou um ponteiro único (já que pode variar, o ideal é por que a função sempre retorne void**)

		// criar MÉTODO DE COPIA DE OBJETO (variavel)

		if(entry->val->OBJECT_SIZE > 1)
		{
			// retorna a referência (ai pode sim ter colaterais)
			return entry->val;
		}
		else
		{
			// copia o objeto atomico
			return copyObject(entry->val);

		}
	}
}


Object* evalTIME_DIRECTIVE(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
{

	TableEntry* entry = lookup(scope,n->leafs[0]);

	// teoricamente é impossível uma time Directive não estar na tabela mas é só um check
	if(!entry)
	{
		fprintf(stderr, "%s NOT DECLARED!", n->leafs[0]);
		exit(-1);
	}
	else
	{
		// retorna cópia numérica das TIME_DIRECTIVES (elas SÃO UNICAS NO CÓDIGO, só alteradas mas não copiadas )
		return createObject(NUMBER_ENTRY,1,entry->val->values);
	}
}



Object* evalDataV(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
{



	//str= evalSTRING(n,  scope, writeSmvTypeTable, controllerSmv);

	//printf("[evalDataV] DATA_V SINTH: %s \n",str);

	//return sint;

	// esse vai ser bem diferente...

	// vai chamar evalParams , e sintetizar um Object Vetor (ou um vetor void que será jogado em um object)
}


// declarar com const ? (me parece "nada haver")
Object* (*executores[80]) (Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv) = {

	evalNUM, evalBOOL, evalSTRING, evalNULL, evalTIME_DIRECTIVE, evalDataV


};


Object* eval(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
{
	//printf("[PostProcess - eval] test compile \n");

	if(n)
	{
			// sintetizado dos filhos
			void** SYNTH_C[n->nchild];	
			// sintetizado dos filhos
			void** SYNTH_L[n->nleafs];
			
			int CODIGO_QUE_REPRESENTA_EXECUTOR_2 = 1;

			if(executores[n->type])
			{
				printf("[PostProcess - eval] eval especifico \n\n");
				return executores[n->type](n,scope,writeSmvTypeTable,controllerSmv);
			}
			else
			{
				printf("[PostProcess - eval] eval genérico \n");
				if(n->children)
				{
					int i;
					for(i= 0; i < n->nchild; i++)
					{
						if(n->children[i]) 
						{
							printf("(%d) %s ",i,n->children[i]->name);
							return eval(n->children[i],scope,writeSmvTypeTable,controllerSmv);
							//SYNTH_O[i] =  process(n->children[i], currentScope); // se tiver filho (desce um nível e resolve as dependencias) 
						}														 // (já criamos Object) resolver dependencias realmente necessário? Parando para pensar podemos acessar o filho imediatamente abaixo do nó em questão e já pegar os valores ! Evita criar mais structs! (pode ficar complexo para alguns casos por outro lado... e fora que inviabiliza tds e vetores)
					}
				}
				// sintetizar as folhas? ou é do eval especifico?
				return NULL;
			}
	}
}

