#include <stdlib.h>
#include "../../headers/TDS.h"


TDS* createTDS(char* name, TDS_TYPE type, Object* valueList, Object* linkedTDSs, int delayed, char* functionRef) {

    TDS* newTDS = (TDS*) malloc(sizeof(TDS));

	newTDS->name = name; // nome sintetizado diretamente do código.
	newTDS->type = type;
	newTDS->DATA_TIME = valueList;
	newTDS->linked = linkedTDSs;
	newTDS->delayed = delayed;
    newTDS->functionRef = functionRef;

	return newTDS;						
}

void* letGoTDS(TDS* tds){

}