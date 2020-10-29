#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../headers/PostProcess.h"

                                                // ex: 1 + 1
char* SmvConversions[] = {"%s:", "{%s};" , "%s, ", "%s %s %s \n", "%s \n", "%s ", "init(%s)", "next(%s)",
                          "init(%s) := %s; \n", "next(%s) := %s; \n" , "case \n\t\t%s \n\tesac; \n",
                          "init(%s) = %s : %s; \n ", "next(%s) = %s : %s; \n", "TRUE : NULL; \n", "%s = %s : %s; \n" };

void updateTime(HeaderSmv* main , STable * writeSmvTypeTable, const char* newValue, int type)
{
    int pos;
    int size;
    TableEntry* entryPosSizeInterval;
    TableEntry* entryPosSizeAssing;

    char* originalInterval;
    char* originalAssign;
    char delim;

    entryPosSizeInterval =  lookup(writeSmvTypeTable,"time");
    pos = *(int*) entryPosSizeInterval->val->values[0];
    originalInterval = main->varBuffer[pos];

    // init
    if(type)
    {
        delim = ' ';
        // mudança da declaração (inicio)
        //loop de 3 iterações no maximo (mas é melhor do que deixar hardcoded)
        while(*originalInterval != delim)
        {
            originalInterval++;
        }
        originalInterval++;
        *originalInterval = newValue[0];

        entryPosSizeAssing =  lookup(writeSmvTypeTable,"init(time)");
        // mudança init
        pos = *(int*) entryPosSizeAssing->val->values[0];
        size = *(int*) entryPosSizeAssing->val->values[1];
        main->assignBuffer[pos][size-2] = newValue[0];

    }
    // final
    else{
        delim = '<';
        // mudança da declaração (final)
        size = *(int*) entryPosSizeInterval->val->values[1];
        originalInterval[size-2] = newValue[0];

        entryPosSizeAssing =  lookup(writeSmvTypeTable,"next(time)");
        //mudança next
        pos = *(int*) entryPosSizeAssing->val->values[0];

        originalAssign = main->assignBuffer[pos];
        while(*originalAssign != delim)
        {
            originalAssign++;
        }
        originalAssign++;
        originalAssign++;
        *originalAssign = newValue[0];
    }
}