#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../headers/Object.h"

const char* mappingEnumObjectType[] =  {
    "NUMBER",
    "BOOLEAN",
    "STRING",
    "T_DIRECTIVE",
    "TDS",
    "null*",
    "FUNCTION",
    "POSxSIZE",
    "TYPE_SET",
};

// talvez esse método não seja mais necessário (seria melhor sempre alocar ANTES DE PASSAR UM INTEIRO (até porque esse inteiro vai ser um atributo sintetizado que vai ser retornado para uma chamada de cima))
// e caso tenha ALGUMA OPERAÇÃO INTERMEDIARIA ENTRE ESSES VALORES (?) ... já que vamos sempre alocar .... acho MELHOR CENTRALIZAR , afinal para isso object foi criado!
// 
// MANTER ESSA FUNÇÃO: E sempre criar objetos para valores sintetizados (caso seja necessário, dar free em valores intermediarios que não venham a ser usados...
void* allocatePtObjects(int type, void* value, Object* newOb) 
{
	if(type == NUMBER_ENTRY || type == T_DIRECTIVE_ENTRY || type == LOGICAL_ENTRY)
	{
		int* pt = malloc(sizeof(int));
		*pt = *(int*) value;
		printf("[allocatePtObjects - numVariants] valor: %d \n",*(int*) pt);
		return pt;
	}
	
	// precisa alocar para strings ? (pode ser que já tenha sido alocado pelo parser) Necessita alocar:  Pode gerar efeito colateral em alguns casos (string pode ser re-usada)
	if(type == LABEL_ENTRY || type == NULL_ENTRY) 
	{
		char* deref = (char*) value;
		char* pt = malloc(newOb->STR+1);
		strcpy(pt, deref);
		printf("[allocatePtObjects - labelVariants] valor: %s (%d)\n",pt,type);
		return pt;
	}


}	


void* allocateTypeSetObjects(int index, void* value)
{
	// (0,1,2)
	if(index < 2)
	{
		int* pt = malloc(sizeof(int));
		*pt = *(int*) value;
		printf("[allocateTypeSetObjects] [i=%d] valor: %d \n",index,*(int*) pt);
		return pt;
	}

	printf("[allocateTypeSetObjects] [i=%d] Hashmap já alocada \n",index);
	return value;
	
}



Object* createObject(int type, int OBJECT_SIZE, void** values) 
{

	Object* newOb = (Object*) malloc(sizeof(Object));

	newOb->type = type;
	newOb->OBJECT_SIZE = OBJECT_SIZE;
	newOb->changedType = 0;
	newOb->binded = 0;
	if(type == LABEL_ENTRY)
	{
		printf("[createObject] string: %s \n",(char*)values[0]);
		newOb->STR = strlen((char*)values[0]); // já que o objeto encapsula arrays também, devemos salvar na verdade um vetor em STR (não é muito importante agora)
	}

	if(OBJECT_SIZE)
	{
		// malloc para garantir que o objeto utilizado não "seja perdido" em chamadas
		void** vo = (void**) malloc(sizeof(void*)*OBJECT_SIZE);
		int i;
		for(i = 0; i< OBJECT_SIZE; i++)
		{
			//vo[i] = values[i];

			// caso do int e tipos mais basicos:  ele chega aqui como um ponteiro para um variavel local (essa região de memoria é perdida depois!)
			// por ser um ponteiro para void que estamos guardando temos duas opções, alocar o ponteiro associado e passar para vo[i]
			// ou alocar antes e passar para esse já alocado! (acho melhor a primeira opção, para centralizar)


			// casos como por exemplo do conjunto de tipos: não precisa de malloc (já é um ponteiro de um objeto alocado a muito tempo )
			//		-> tupla ponteiro smv (indiceHeader, tamanhoPalavra, Hashmap)

			if(type == TYPE_SET || type == POSxSIZE)
			{
				vo[i] = allocateTypeSetObjects(i,values[i]);
			}
			else
			{
				vo[i] = allocatePtObjects(type,values[i],newOb);
			}

		}
		newOb->values = vo;	
	}


	//int info = newOb == NULL ?  1 : 0;

	//printf("[DEBUG - createObject] info: %d \n", info);

	//printObject(newOb);


	return newOb;

}


void printObject(Object* o)
{

	int info = o == NULL ?  1 : 0;
	//printf("[DEBUG - printObject] info: %d \n", info);
	

	if(!info)
	{
		int i;
		//printf("[DEBUG - printObject] tipoDetectado: %s \n", mappingEnumObjectType[o->type]);
		if(o->type == NUMBER_ENTRY || o->type == T_DIRECTIVE_ENTRY || o->type == TYPE_SET || o->type == POSxSIZE)
		{
			printf("(");
			printf("%s :: ", mappingEnumObjectType[o->type]);
			for(i = 0; i < o->OBJECT_SIZE; i++)
			{

				if((o->type != TYPE_SET) || (o->type == TYPE_SET &&  i < o->OBJECT_SIZE-1) )
				{
					//printf("[printObject] index on deref: %d \n",i);
					int deref = *(int*) o->values[i];
					printf("%d",deref);
					if(i != o->OBJECT_SIZE-1)
					{
						printf(", ");
					}
				}
			}		
			
		}

		// logical == converter para numeros? 
		if(o->type== LOGICAL_ENTRY)
		{	
				for(i = 0; i < o->OBJECT_SIZE; i++)
				{
					int derefboolean = *(int*) o->values[i];
					if(derefboolean)
					{
						printf(" (%s, %s) \n",mappingEnumObjectType[o->type],"true");
					}
					else
					{
						printf(" (%s, %s) \n",mappingEnumObjectType[o->type],"false");	
					}
				}					

		}

		if(o->type == LABEL_ENTRY || o->type == NULL_ENTRY)
		{
			for(i = 0; i < o->OBJECT_SIZE; i++)
			{
				const char* valor = (char*) o->values[i];
				printf(" (%s, '%s' ) \n",mappingEnumObjectType[o->type],valor);	
				
			}				
		}

		if(o->type == TDS_ENTRY){
			// TODO (struct TDS)
		}
    }
    else
    {
    	printf("null \n");
    }
}


void letgoObject(Object* o, int always)
{
	if(o && (o->binded || always))
	{
		int i;
		for (i = 0; i < o->OBJECT_SIZE; i++)
		{
			if(o->type == TYPE_SET)
			{
				if(i < 2)
				{
					free(o->values[i]);
				}
				// usar o free da tabela de simbolos (em outra localidade anteriormente?)
			}
			else if(o->type == TDS_ENTRY)
			{
				// TODO PARA TDS
			}
			else
			{
				free(o->values[i]);
			}
		}
	}
}


Object* copyObject(Object* o) 
{
	Object* newOb = createObject(o->type, o->OBJECT_SIZE, o->values);
	return newOb;
}

void updateObject(Object* o, void** any, int any_type, int object_size, int index, int prop)
{
	// devemos também verificar se o tamanho mudou (agora é um vetor , etc ...)


	free(o->values[index]);
	o->values[index] = NULL;
	// free object em CASO DE VETORES (ainda temos que pensar um pouco mais nisso)
	// caso para any sendo de valor único
	if(object_size == 1)
	{
		//printf("[updateObject] indexUpdate: %d \n",index);
		void* newPt = allocatePtObjects(any_type,any[0],o);
		o->values[index] = newPt;
	}

	if(o->type != any_type)
	{
		printf("[updateObject] type-> %s \n",mappingEnumObjectType[any_type]);
		o->type = any_type;
		o->changedType = 1;
	}
}

