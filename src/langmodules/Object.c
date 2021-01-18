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
    "WRITE_SMV_INFO",
    "TYPE_SET",
};

// talvez esse método não seja mais necessário (seria melhor sempre alocar ANTES DE PASSAR UM INTEIRO (até porque esse inteiro vai ser um atributo sintetizado que vai ser retornado para uma chamada de cima))
// e caso tenha ALGUMA OPERAÇÃO INTERMEDIARIA ENTRE ESSES VALORES (?) ... já que vamos sempre alocar .... acho MELHOR CENTRALIZAR , afinal para isso object foi criado!
// 
// MANTER ESSA FUNÇÃO: E sempre criar objetos para valores sintetizados (caso seja necessário, dar free em valores intermediarios que não venham a ser usados...
void* allocatePtObjects(int type, void* value, Object* newOb,int index)
{
	if(type == NUMBER_ENTRY || type == T_DIRECTIVE_ENTRY || type == LOGICAL_ENTRY || type == WRITE_SMV_INFO)
	{
		int* pt = malloc(sizeof(int));
		*pt = *(int*) value;
		printf("[allocatePtObjects - numVariants] valor: %d \n",*(int*) pt);
		printf("[allocatePtObjects - numVariants] success \n");
		return pt;
	}
	
	// precisa alocar para strings ? (pode ser que já tenha sido alocado pelo parser) Necessita alocar:  Pode gerar efeito colateral em alguns casos (string pode ser re-usada)
	if(type == LABEL_ENTRY || type == NULL_ENTRY) 
	{
		char* deref = (char*) value;
		newOb->STR[index] = strlen(value);
		char* pt = malloc(sizeof(char)*newOb->STR[index]+1);
		strcpy(pt, deref);
		printf("[allocatePtObjects - labelVariants] valor: %s (%d)\n",pt,type);
		return pt;
	}


}	


void* allocateTypeSetObjects(int index, void* value)
{
	// (0,1,2)
	if(index < 2 )
	{
		int* pt = malloc(sizeof(int));
		*pt = *(int*) value;
		printf("[allocateTypeSetObjects] [i=%d] valor: %d \n",index,*(int*) pt);
		return pt;
	}

	printf("[allocateTypeSetObjects] [i=%d] Hashmap já alocada \n",index);
	return value;
	
}


Object *createObject(int type, int OBJECT_SIZE, void **values, int timeContext, char *BIND)
{

	Object* newOb = (Object*) malloc(sizeof(Object));

	newOb->type = type;
	newOb->OBJECT_SIZE = OBJECT_SIZE;
	newOb->redef = 0;
	newOb->timeContext = timeContext;
    if(BIND){
        newOb->SINTH_BIND = malloc(sizeof(char)*strlen(BIND) + 1);
        strcpy(newOb->SINTH_BIND, BIND);
    }
    else{
//      newOb->ORIGINAL_BIND = NULL;
        newOb->SINTH_BIND = NULL;
    }

    if(type == LABEL_ENTRY)
	{
		newOb->STR = malloc(sizeof(int)*OBJECT_SIZE);
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

			if(type == TYPE_SET)
			{
				vo[i] = allocateTypeSetObjects(i,values[i]);
			}
			else
			{
				vo[i] = allocatePtObjects(type,values[i],newOb,i);
			}

		}
		newOb->values = vo;	
	}


	int info = newOb == NULL ?  1 : 0;

	printf("[DEBUG - createObject] info: %d \n", info);

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
		if(o->type == NUMBER_ENTRY || o->type == T_DIRECTIVE_ENTRY || o->type == TYPE_SET || o->type == WRITE_SMV_INFO)
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


void letgoObject(Object *o)
{
	if(o)
	{
		int i;
		for (i = 0; i < o->OBJECT_SIZE; i++)
		{
			if(o->type == TYPE_SET)
			{
				if(i < 2)
				{
					free(o->values[i]);  // usar o free da tabela de simbolos para i == 3 (em outra localidade anteriormente)
				}
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
	Object* newOb = createObject(o->type, o->OBJECT_SIZE, o->values, o->timeContext, o->SINTH_BIND);
	return newOb;
}


void updateObjectCell(Object* o, void** any, int any_type ,int object_size, int index, int prop)
{
    //printf("[updateObject] indexUpdate: %d \n",index);
    if(object_size == 1)
    {
        o->values[index] = NULL;
        void* newPt = allocatePtObjects(any_type,any[0],o,index);
        o->values[index] = newPt;
    }

}




void updateObject(Object *o, void **any, int any_type, int object_size, int index, int prop, int contextChange)
{
    // caso x = y (copia o valor, diferente de array com indice 0, isto é, x[0](unico) )
	if(index == -1 && prop == -1)
	{
            // vale para ambos os casos (mesmo que o seja um vetor sendo sobrescrito ou variavel comum)
            int i;
            for (i = 0; i < o->OBJECT_SIZE; i++)
            {
                free(o->values[i]);
                o->values[i] = NULL;
            }
            if(object_size > 1)
            {
                // passa referencia
                o->values = any;
                o->OBJECT_SIZE = object_size;
            }
            updateObjectCell(o,any,any_type,object_size,0,-1);
	}
    // caso x[i] = y
	else if(index != -1 ){
        free(o->values[index]);
        o->values[index] = NULL;
        updateObjectCell(o,any,any_type,object_size,index,-1);
	}

	int typeChanged = 0;
	if(o->type != any_type && o->type != any_type)
	{
		printf("[updateObject] -------type-change----> %s \n",mappingEnumObjectType[any_type]);
		o->type = any_type;
		typeChanged = 1;
	}
	int oldRedef = o->redef;
    o->redef = typeChanged || (contextChange != o->timeContext)? o->redef : o->redef+1;
    o->timeContext = contextChange == o->timeContext? o->timeContext : contextChange;
}

