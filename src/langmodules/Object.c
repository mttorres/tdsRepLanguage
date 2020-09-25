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
};


Object* createObject(int type, int OBJECT_SIZE, void** values) 
{

	Object* newOb = (Object*) malloc(sizeof(Object));

	newOb->type = type;
	newOb->OBJECT_SIZE = OBJECT_SIZE;
	if(OBJECT_SIZE)
	{
		void** vo = (void**) malloc(sizeof(void*)*OBJECT_SIZE);
		int i;
		for(i = 0; i< OBJECT_SIZE; i++)
		{
			vo[i] = values[i];
			//printf("[DEBUG] createObject valor: %d \n",*(int*) values[i]);
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
		if(o->type == NUMBER_ENTRY || o->type == T_DIRECTIVE_ENTRY)
		{
			//printf("[DEBUG] printObject: NUMBER \n ");
			for(i = 0; i < o->OBJECT_SIZE; i++)
			{
				int deref = *(int*) o->values[i];
				printf(" (%s, %d) \n",mappingEnumObjectType[o->type],deref);
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

