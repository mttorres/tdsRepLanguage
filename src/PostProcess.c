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


void* evalNUM(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
{

	int* sint = (int*) malloc(sizeof(int));
	*sint= atoi(n->leafs[0]);

	printf("[evalNUM] SINTH: %d \n",*sint);
	
	//void* ip[] = {&sint[0]};

	// vai precisar alocar esse inteiro...
	//Object* o = createObject(NUMBER_ENTRY, 1 , ip); 
	
	//printObject(o);

	// LIBERAR A LEAF ? 

	return sint;
}

void* evalBOOL(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
{

	int* sint = (int*) malloc(sizeof(int));

	char* trueString = "true";

	*sint= strstr(n->leafs[0],trueString)? 1 : 0;

	printf("[evalBOOL] SINTH: %d \n",*sint);
	
	//void* ip[] = {&sint[0]};

	// vai precisar alocar esse inteiro...
	//Object* o = createObject(NUMBER_ENTRY, 1 , ip); 
	
	//printObject(o);

	// LIBERAR A LEAF ? 

	return sint;
}


void* evalSTRING(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
{

	char* str =  (char*) malloc(sizeof(char)*strlen(n->leafs[0])+1);
	// não precisa de malloc (apesar que se mudarem essa string pode rolar efeito colateral nas demais que usam essa que veio da arvore "ponto de falha")
	
	strcpy(str, n->leafs[0]);

	printf("[evalSTRING] SINTH: %s \n",str);
	
	//void* ip[] = {&sint[0]};

	// vai precisar alocar esse inteiro...
	//Object* o = createObject(NUMBER_ENTRY, 1 , ip); 
	
	//printObject(o);

	// LIBERAR A LEAF ? 

	return str;
}

void* evalNULL(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
{

	char* str; // não precisa de malloc (pegar a string NULL justamente para "facilitar" o reconhecimento)
	// se eu interpretar como "NULL" do C mesmo podemos ter problemas(?)

	str= evalSTRING(n,  scope, writeSmvTypeTable, controllerSmv);

	printf("[evalDataV] SINTH: %s \n",str);

	return str;
}


void** evalIDVAR(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
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
		// é um COPIA DE VARIAVEL , ou uma referencia a uma ED já existente (a ideia é fazer a mesma coisa com TDS ao invés de copiar por razões óbvias)
		
		// precisamos alocar um vetor de ponteiros para void ou um ponteiro único (já que pode variar, o ideal é por que a função sempre retorne void**)

		// criar MÉTODO DE COPIA DE OBJETO (variavel)

		void** varVal = (void**) malloc(sizeof(void*)*entry->val->OBJECT_SIZE);
		int i;
		for (i = 0; i < entry->val->OBJECT_SIZE; i++)
		{
			//varVal[i] = (entry->val->values[i]);
			// TEM QUE PRIMEIRO, castar para o tipo do ponteiro original, dereferencia E AI SIM COPIAR O VALOR

			// por enquanto vamos pensar em vetores DE TIPO ÚNICO POR VEZ
			if(entry->val->type == NUMBER_ENTRY || entry->val->type == T_DIRECTIVE_ENTRY)
			{
				int* val = (int*) entry->val->values[i];
			}



		}
		return varVal;
	}
}



void* evalDataV(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
{



	//str= evalSTRING(n,  scope, writeSmvTypeTable, controllerSmv);

	//printf("[evalDataV] DATA_V SINTH: %s \n",str);

	//return sint;

	// esse vai ser bem diferente...

	// vai chamar evalParams , e sintetizar um Object Vetor (ou um vetor void que será jogado em um object)
}


// declarar com const ? (me parece "nada haver")
void* (*executores[80]) (Node*, STable*) = {processFuncTest1, processFuncTest2};


void* eval(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
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
				printf("[PostProcess - eval] eval especifico \n");
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
							printf("(%d)    ",i);
							//SYNTH_O[i] =  process(n->children[i], currentScope); // se tiver filho (desce um nível e resolve as dependencias) 
						}														 // (já criamos Object) resolver dependencias realmente necessário? Parando para pensar podemos acessar o filho imediatamente abaixo do nó em questão e já pegar os valores ! Evita criar mais structs! (pode ficar complexo para alguns casos por outro lado... e fora que inviabiliza tds e vetores)
					}
				}
			}
	}
}

