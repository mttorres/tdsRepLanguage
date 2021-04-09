#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../headers/Object.h"

const char* mappingEnumObjectType[] =  {
    "NUMBER",
    "BOOLEAN",
    "LABEL",
    "T_DIRECTIVE",
    "TDS",
    "null*",
    "TIME_COMPONENT",
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
	if(type == NUMBER_ENTRY || type == T_DIRECTIVE_ENTRY || type == LOGICAL_ENTRY || type == WRITE_SMV_INFO )
	{
		int* pt = malloc(sizeof(int));
		*pt = *(int*) value;
		//printf("[allocatePtObjects - numVariants] valor: %d \n",*(int*) pt);
		//printf("[allocatePtObjects - numVariants] success \n");
		return pt;
	}
	// precisa alocar para strings ? (pode ser que já tenha sido alocado pelo parser) Necessita alocar:  Pode gerar efeito colateral em alguns casos (string pode ser re-usada)
	if(type == LABEL_ENTRY || type == NULL_ENTRY) 
	{
		char* deref = (char*) value;
		newOb->STR = strlen(value);
		char* pt = malloc(sizeof(char)*newOb->STR+1);
		strcpy(pt, deref);
		//printf("[allocatePtObjects - labelVariants] valor: %s (%d)\n",pt,type);
		return pt;
	}
    if(newOb->SINTH_BIND){
        fprintf(stderr, "FAIL IN ALLOCATE VALUES FOR %s ! \n",newOb->SINTH_BIND);
    }
    else{
        fprintf(stderr, "FAIL IN ALLOCATE VALUES FOR CURRENT OBJECT! \n");
    }
    exit(-1);
}	


void* allocateTypeSetObjects(int index, void* value)
{
	// (0,1,2)
	if(index < 2 )
	{
		int* pt = malloc(sizeof(int));
		*pt = *(int*) value;
	//	printf("[allocateTypeSetObjects] [i=%d] valor: %d \n",index,*(int*) pt);
		return pt;
	}
	//printf("[allocateTypeSetObjects] [i=%d] Hashmap já alocada \n",index);
	return value;
	
}

Object* setUpForNewObject(object_type type, int OBJECT_SIZE, int timeContext, char* BIND){
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
        newOb->SINTH_BIND = NULL;
    }

    if(OBJECT_SIZE) {
        // malloc para garantir que o objeto utilizado não "seja perdido" em chamadas
        void **vo = (void **) malloc(sizeof(void *) * OBJECT_SIZE);
        newOb->values = vo;
    }
    else{
        if(newOb->type != NULL_ENTRY){
            fprintf(stderr, "[createObject] Error during values allocation. Zero Sized object \n");
            exit(-1);
        }
    }
    return newOb;
}

Object *createObject(object_type type, int OBJECT_SIZE, void **values, int timeContext, char *BIND, void *TYPE_SMV_INFO)
{
    Object* newOb = setUpForNewObject(type,OBJECT_SIZE,timeContext,BIND);
    newOb->type_smv_info = TYPE_SMV_INFO;
 /*
    if(type == LABEL_ENTRY)
    {
        newOb->STR = malloc(sizeof(int)*OBJECT_SIZE);
    }
    else{
        newOb->STR = NULL;
    }
*/
    newOb->aList = 0;
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
				newOb->values[i] = allocateTypeSetObjects(i,values[i]);
			}
			else
			{
                newOb->values[i] = allocatePtObjects(type,values[i],newOb,i);
			}
    }
    //printf("[DEBUG - createObject] info: %d \n", info);
	//printObject(newOb);
	return newOb;
}

Object* createObjectDS(object_type type, int OBJECT_SIZE, void ** values, int timeContext, char *BIND, int aList) {
    Object* newOb = setUpForNewObject(type,OBJECT_SIZE,timeContext,BIND);
    newOb->aList = aList;
    newOb->type_smv_info = NULL;
    int i;
    for (i = 0; i < OBJECT_SIZE; i++) {
        newOb->values[i] = values[i];
    }
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
						printf(" (%s :: %s) \n",mappingEnumObjectType[o->type],"true");
					}
					else
					{
						printf(" (%s :: %s) \n",mappingEnumObjectType[o->type],"false");
					}
				}					

		}

		if(o->type == LABEL_ENTRY)
		{
			for(i = 0; i < o->OBJECT_SIZE; i++)
			{
				const char* valor = (char*) o->values[i];
				printf(" (%s :: '%s' ) \n",mappingEnumObjectType[o->type],valor);
				
			}				
		}
		if(o->type == NULL_ENTRY){
            printf(" (%s :: '%s' ) \n",mappingEnumObjectType[LABEL_ENTRY],"NULL");
		}

		if(o->type == TDS_ENTRY){
			// TODO (struct TDS)
		}
	} else
    {
    	printf("null \n");
    }
}


/**
 * Desaloca a lista de objetos auxiliares time-component. Sendo esses da forma:
 * Object --HEAD-> { {int,PATH} , ..., {int,PATH} }
 * @param o
 */
void letGoTimeComponent(Object* o){
    if(o->OBJECT_SIZE < 2 && !o->aList){
        fprintf(stderr, "[letGoTimeComponent] Possible error while freeing memory \n");
        //exit(-1);
    }
    int i;
    for(i = 0; i < o->OBJECT_SIZE; i++) {
        Object* currentComponent = o->values[i];
        free(currentComponent->values[0]);
        currentComponent->values[1] = NULL;
        free(currentComponent->values);
        currentComponent->values = NULL;
        if(currentComponent->SINTH_BIND){
            free(currentComponent->SINTH_BIND);
        }
        currentComponent->SINTH_BIND = NULL;
        free(currentComponent);
    }
}


void letgoObject(Object *o)
{
	if(o)
	{
        // OBJETOS atômicos da linguagem e listas dos tipos padrões
        if(o->type != TDS_ENTRY && o->type != TIME_COMPONENT){
            int i;
            for (i = 0; i < o->OBJECT_SIZE; i++)
            {
                if(o->OBJECT_SIZE == 1 || o->type == WRITE_SMV_INFO || o->type == TYPE_SET) {
                    free(o->values[i]);  // não precisa mais do mapeamento para type-sets (vai ser só um pos x size agora que tem type_smv_info um type-set)
                }
                // listas padrões
                else{
                    if(o->OBJECT_SIZE > 1){
                        letgoObject(o->values[i]);
                    }
                }
                o->values[i] = NULL;
            }
	    }
        else{
                if(o->type == TDS_ENTRY){
                    // let go tds
                }
                if(o->type == TIME_COMPONENT){
                    // let go time component
                    letGoTimeComponent(o);
                }
        }
	}
    free(o->values);
    o->values = NULL;
	if(o->SINTH_BIND){
	    free(o->SINTH_BIND);
	}
    o->SINTH_BIND = NULL;
    if(o->type == SMV_PORTS){
        //letgo min max verificando se o type_smv_info não é null
        if(o->type_smv_info){
            letGoTypeMinMax(o->type_smv_info);
        }
    }
    if(o->type == TYPE_SET){
        //let go type-set
        if(o->type_smv_info){
            letGoTypeSet(o->type_smv_info);
        }
    }
    o->type_smv_info = NULL;
	free(o);
}


Object* copyObject(Object* o) 
{
    if(o){
        Object* newOb = createObject(o->type, o->OBJECT_SIZE, o->values, o->timeContext, o->SINTH_BIND, NULL);
        return newOb;
    }
    return NULL;
}

void updateNullRef(Object *var, Object *expr){
    var->type = expr->type;
    // responsabilidade do metodo de atualizaçao de variavel logo depois
    if(!expr->aList){
        var->values = (void **) malloc(sizeof(void *) * 1);
    }
    if(expr->SINTH_BIND){
        char* newBind = realloc(var->SINTH_BIND,strlen(expr->SINTH_BIND)+1);
        if(!newBind){
            fprintf(stderr, "[updateNullRef] Error while allocating bind \n");
            exit(-1);
        }
        strcpy(newBind,expr->SINTH_BIND);
        var->SINTH_BIND = newBind;
    }
}


void updateObjectCell(Object* o, void** any, int any_type ,int object_size, int index, int prop)
{
    //printf("[updateObject] indexUpdate: %d \n",index);
    free(o->values[index]); // libera o indice atual, sobrescrevendo o valor
    o->values[index] = NULL;
    void* newPt;

    if(object_size == 1)
    {
        newPt = allocatePtObjects(any_type,any[0],o,index);
        o->values[index] = newPt;
    }
}

/**
 * prototipo para listas
 */
void updateObjectReference(){

}



void updateObject(Object *o, void **any, int any_type, int object_size, int index, int prop, int contextChange)
{
    // caso x = y (copia o valor, diferente de array com indice 0, isto é, x[0](unico) )
	if(index == -1 && prop == -1)
	{
            // vale para ambos os casos (mesmo que o seja um vetor sendo sobrescrito ou variavel comum)
            // se um vetor é sobrescrito, devemos ou não limpar seus valores? (LIMPAR PODERIA TER EFEITOS COLATERAIS EM QUEM REFERENCIA ELE)
            // não limpar poderia acarretar em objetos com memória solta (apesar que no final de cada escopo eles iam ser inevitavelmente liberados)
            // PORÉM, e se nenhum objeto referenciar essa memória perdida? Vai ter que ser liberada em algum momento (final do programa?)
            // mover a limpeza para object o método de object cell e deixar mais simples
/*
            int i;
            for (i = 0; i < o->OBJECT_SIZE; i++)
            {
                free(o->values[i]);
                o->values[i] = NULL;
            }
*/
            if(object_size > 1)
            {
                // passa referencia (é para arrays)
                updateObjectReference();
//              o->values = any;
//              o->OBJECT_SIZE = object_size;
            }
            else{
                updateObjectCell(o,any,any_type,object_size,0,-1);
            }
	}
    // caso x[i] = y
	else if(index != -1 ){
        updateObjectCell(o,any,any_type,object_size,index,-1);
	}

//	int typeChanged = 0;
	if(o->type != any_type)
	{
		printf("[updateObject] -------type-change----> %s \n",mappingEnumObjectType[any_type]);
		exit(-1);
		//typeChanged = 1;
	}
	//int oldRedef = o->redef;
    //o->redef = typeChanged || (contextChange != o->timeContext)? o->redef : o->redef+1;
    o->timeContext = contextChange == o->timeContext? o->timeContext : contextChange;
}

/**
 * Adiciona um membro novo a uma lista, de maneira que seu novo limite de tamanho seja redefinido.
 * @param original a lista original ou objeto único
 * @param newMember o objeto de mesmo tipo que deve ser adicionado
 * @SideEffects:  Realoca o values de original.
 */
void updateListObjectBound(Object* original, Object* newMember) {
    int mergedSize = original->OBJECT_SIZE +1; // lista + novo ou novo + novo (n ou 2)
    void** mergedValues = realloc(original->values,mergedSize);
    if(!mergedValues) {
        fprintf(stderr, "[updateListObjectBound] ERROR IN ALLOCATION \n");
        exit(-1);
    }
    original->values = mergedValues;
    original->values[original->OBJECT_SIZE] = newMember; // [n-1] ou [1]
    original->OBJECT_SIZE++;
}

Object * mergeGenericList(Object* LEFT_COMPONENT, Object* RIGHT_COMPONENT){

    object_type selectedType;
    // permite juntar tipos diferentes nesse caso
    if(LEFT_COMPONENT->type == RIGHT_COMPONENT->type){
        selectedType = LEFT_COMPONENT->type;
    }
    else{
        //Object* MEMBER_OF_LIST = (Object*) LEFT_COMPONENT->values[0];
        //if(LEFT_COMPONENT->OBJECT_SIZE > 1 && MEMBER_OF_LIST->type != RIGHT_COMPONENT->type) {
            fprintf(stderr, "ERROR: Array structure with different data types.");
            exit(-1);
        //}
        //else{
        //    selectedType = GEN_LIST;
        //}
    }
    Object* newList = LEFT_COMPONENT;
    if(!LEFT_COMPONENT->aList){
        void* vp[] = {LEFT_COMPONENT,RIGHT_COMPONENT};
        newList = createObjectDS(selectedType,2,vp,LEFT_COMPONENT->timeContext,LEFT_COMPONENT->SINTH_BIND,1);
    }
    else{
        updateListObjectBound(newList,RIGHT_COMPONENT);
    }
    return newList;
}

