#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../headers/constants.h"
#include "../../headers/Object.h"

const char* mappingEnumObjectType[] =  {
    "NUMBER",
    "LOGICAL",
    "LABEL",
    "TDS",
    "T_DIRECTIVE",
    "SMV_POINTER",
};


void* allocatePtObjects(int type, void* value) 
{
	if(type == NUMBER_ENTRY || type == T_DIRECTIVE_ENTRY || type == SMV_POINTER)
	{
		int* pt = malloc(sizeof(int));
		*pt = *(int*) value;
		return pt;
	}
	
	// precisa alocar para strings ? (pode ser que já tenha sido alocado pelo parser)
	if(type == LABEL_ENTRY) 
	{
		//TODO (ao fazer o pós processamento)
	}


}	


Object* createObject(int type, int OBJECT_SIZE, void** values) 
{

	Object* newOb = (Object*) malloc(sizeof(Object));

	newOb->type = type;
	newOb->OBJECT_SIZE = OBJECT_SIZE;
	if(OBJECT_SIZE)
	{
		// malloc para garantir que o objeto utilizado não "seja perdido" em chamadas
		void** vo = (void**) malloc(sizeof(void*)*OBJECT_SIZE);
		int i;
		for(i = 0; i< OBJECT_SIZE; i++)
		{
			//vo[i] = values[i];

			// caso do int:  ele chega aqui como um ponteiro para um variavel local (essa região de memoria é perdida depois!)
			// por ser um ponteiro para void que estamos guardando temos duas opções, alocar o ponteiro associado e passar para vo[i]
			// ou alocar antes e passar para esse já alocado! (acho melhor a primeira opção, para centralizar)
			vo[i] = allocatePtObjects(type,values[i]);			
			printf("[createObject] valor: %d \n",*(int*) values[i]);
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

	//printf("[DEBUG - createObject] info: %d \n", info);
	

	if(!info)
	{
		int i;
		if(o->type == NUMBER_ENTRY || o->type == T_DIRECTIVE_ENTRY || o->type == SMV_POINTER)
		{
			for(i = 0; i < o->OBJECT_SIZE; i++)
			{
				int deref = *(int*) o->values[i];
				printf(" (%s, %d)",mappingEnumObjectType[o->type],deref);
				if(i != o->OBJECT_SIZE-1)
				{
					printf(",");
				}
			}
			printf("\n");			
			
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

		if(o->type == LABEL_ENTRY)
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

