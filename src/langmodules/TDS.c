#include <stdlib.h>
#include <stdio.h>
#include "../../headers/TDS.h"


TDS * createTDS(char *name, TDS_TYPE type, Object *valueList, int delayed, char *functionRef, int I_INTERVAL, int F_INTERVAL) {

    TDS* newTDS = (TDS*) malloc(sizeof(TDS));

	newTDS->name = name; // nome sintetizado diretamente do código.
	newTDS->type = type;
	newTDS->DATA_SPEC = valueList;

	newTDS->DATA_TIME = malloc(sizeof(Object*)*F_INTERVAL+1);
    int i;
    // Por padrão, ainda não sabemos qual valor por tempo de cada parte dessa tds (vai ser avaliado no código via lazy)
    for (i = 0; i < F_INTERVAL; i++) {
        newTDS->DATA_TIME[i] = NULL;
    }

	newTDS->TOTAL_DEPENDENTS_PT = 0;
    newTDS->TOTAL_DEPENDENCIES_PT = type == TDS_DEPEN? 0 : -1;
    newTDS->linkedDependent = NULL;

	newTDS->delayed = delayed;
    newTDS->I_INTERVAL = I_INTERVAL;
    newTDS->F_INTERVAL = F_INTERVAL;
	newTDS->functionRef = functionRef;
    newTDS->SMV_REF = -1;

    newTDS->COMPONENT_TIMES = type == DATA_LIST?  malloc(sizeof(int)*F_INTERVAL+1) : NULL;
    if(newTDS->COMPONENT_TIMES){
        for (i = 0; i < F_INTERVAL; i++) {
            newTDS->COMPONENT_TIMES[i] = -1;
        }
    }

	return newTDS;						
}

void addTdsDependent(TDS* tds, TDS* dependent){
    if(!tds->linkedDependent && !tds->TOTAL_DEPENDENTS_PT){
        tds->linkedDependent = malloc(sizeof(TDS*) * MAX_DEPEND); // primeiro no maximo 150 dependencias
        // salva a primeira
        tds->linkedDependent[tds->TOTAL_DEPENDENTS_PT] = dependent;
        tds->TOTAL_DEPENDENTS_PT++;
    }
    else{
        // DEVE REALIZAR REALLOC
        if(tds->TOTAL_DEPENDENTS_PT + 1 > MAX_DEPEND ){
            TDS** newRef = realloc(tds->linkedDependent, (tds->TOTAL_DEPENDENTS_PT + 1) * 2);
            if(!newRef) {
                fprintf(stderr, "[addTdsDependent] ERROR IN ALLOCATION \n");
                exit(-1);
            }
            tds->linkedDependent = newRef;
            tds->linkedDependent[tds->TOTAL_DEPENDENTS_PT] = dependent;
            tds->TOTAL_DEPENDENTS_PT++;
        }
    }
    dependent->TOTAL_DEPENDENCIES_PT++;
}


void* letGoTDS(TDS* tds){

}