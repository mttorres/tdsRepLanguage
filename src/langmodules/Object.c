#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../headers/Object.h"



Object* createObject(char* name, int type, int OBJECT_SIZE, void** values) 
{


	Object* o = (Object*) malloc(sizeof(Object));
	o->name = name;
	o->type = type;
	o->dataList = dataList;
	if(OBJECT_SIZE){
		void** vo = (void**) malloc(sizeof(void*)*OBJECT_SIZE);
	}

}


void printObject(Object* o)
{
	if(o) 
	{

		if(o->type == NUMBER_ENTRY || o->type == T_DIRECTIVE_ENTRY)
		{
			printf("%s : (%s, %d, ) \n",o->name,mappingObjectType[o->type],*(int *)o->val);
		}

		// logical == converter para numeros? 
		if(e->val->type== LOGICAL_ENTRY)
		{
			int boolean = *(int * ) e->val;
			if(boolean)
			{

				printf("%s : (%s, %d, ) \n",o->name,mappingObjectType[o->type],"true");
			}
			else
			{
				printf("%s : (%s, %d, ) \n",o->name,mappingObjectType[o->type],"false");	
			}
		}

		if(e->type == LABEL_ENTRY)
		{
			const char* valor; 
			valor = (char*) e->val;
			printf("%s : (%s, %d, ) \n",o->name,mappingObjectType[o->type],valor);	
		}

		if(e->type == TDS_ENTRY){
			// TODO (struct TDS)
		}
    }
    else
    {
    	printf("null \n");
    }
}