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
    "TYPE_SET",
};


void* allocatePtObjects(int type, void* value) 
{
	if(type == NUMBER_ENTRY || type == T_DIRECTIVE_ENTRY)
	{
		int* pt = malloc(sizeof(int));
		*pt = *(int*) value;
		printf("[allocatePtObjects] valor: %d \n",*(int*) pt);
		return pt;
	}
	
	// precisa alocar para strings ? (pode ser que já tenha sido alocado pelo parser)
	if(type == LABEL_ENTRY) 
	{
		//TODO (ao fazer o pós processamento)
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

			if(type == TYPE_SET)
			{
				vo[i] = allocateTypeSetObjects(i,values[i]);
			}
			else
			{
				vo[i] = allocatePtObjects(type,values[i]);
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
		if(o->type == NUMBER_ENTRY || o->type == T_DIRECTIVE_ENTRY || o->type == TYPE_SET)
		{

			for(i = 0; i < o->OBJECT_SIZE; i++)
			{

				if((o->type != TYPE_SET) || (o->type == TYPE_SET &&  i < o->OBJECT_SIZE-1) )
				{
					//printf("[printObject] index on deref: %d \n",i);
					int deref = *(int*) o->values[i];
					printf(" (%s, %d)",mappingEnumObjectType[o->type],deref);
					if(i != o->OBJECT_SIZE-1)
					{
						printf(",");
					}
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

