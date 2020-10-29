#ifndef POSP_H

#define POSP_H

#include "textManager.h"
#include "HeaderSmv.h"
#include "STable.h"
#include "Node.h"
#include "Object.h"

/*
 * Atualiza o intervalo de tempo  -> "time: 0..3;" e
 * o init(time) ou next(time) dependendo se for I_TIME ou F_TIME
 * */

void updateTime(HeaderSmv* main , STable * writeSmvTypeTable, const char* newValue, int type);


#endif

