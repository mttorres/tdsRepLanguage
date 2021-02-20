#include <stdlib.h>
#include "../../headers/TDS.h"


TDS *createTDS(char *name, TDS_TYPE type, Object *valueList, Object *linkedTDSs, int delayed, char *functionRef,
               int I_INTERVAL, int F_INTERVAL) {

    TDS* newTDS = (TDS*) malloc(sizeof(TDS));

	newTDS->name = name; // nome sintetizado diretamente do código.
	newTDS->type = type;
	newTDS->DATA_TIME = valueList;
	newTDS->linked = linkedTDSs;
	newTDS->delayed = delayed;
    newTDS->I_INTERVAL = I_INTERVAL;
    newTDS->F_INTERVAL = F_INTERVAL;
	newTDS->functionRef = functionRef;
    newTDS->COMPONENT_TIMES = type == DATA_LIST?  malloc(sizeof(int)*F_INTERVAL) : NULL;
    newTDS->SMV_REF = -1;
    if(newTDS->COMPONENT_TIMES){
        int i;
        for (i = 0; i < F_INTERVAL; i++) {
            newTDS->COMPONENT_TIMES[i] = 0;
        }
    }

	return newTDS;						
}

void* letGoTDS(TDS* tds){

}