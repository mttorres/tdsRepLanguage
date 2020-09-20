#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../headers/TDSvar.h"
#include "../../headers/constants.h"


TDSvar* createTDS(char* name, int type, int I_TIME, int F_TIME, int C_TIME, int linked, int delayed) {
	
	TDSvar* newTDS = (TDSvar*) malloc(sizeof(TDSvar));

	newTDS->name = name;
	newTDS->type = type;
	newTDS->I_TIME = I_TIME;
	newTDS->F_TIME = F_TIME;
	newTDS->C_TIME = C_TIME;
	newTDS->linked = linked;
	newTDS->delayed = delayed;

	return newTDS;						
}