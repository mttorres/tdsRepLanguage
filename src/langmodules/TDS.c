#include <stdlib.h>
#include <stdio.h>
#include "../../headers/TDS.h"


TDS* createTDS(char *name, TDS_TYPE type, void *valueList, int delayed, int I_INTERVAL, int F_INTERVAL, void *limitCondition) {

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

    newTDS->TOTAL_DEPENDENCIES_PT = type == TDS_DEPEN? 0 : -1;
    newTDS->linkedDependency = NULL;
    newTDS->LAST_DELAYED_ACCEPT_TIME = type == TDS_DEPEN && delayed? I_INTERVAL+1 : -1;

	newTDS->delayed = delayed;
    newTDS->I_INTERVAL = I_INTERVAL;
    newTDS->F_INTERVAL = F_INTERVAL;
    newTDS->SMV_REF = -1;

    newTDS->COMPONENT_TIMES = type == DATA_LIST?  malloc(sizeof(int)*F_INTERVAL+1) : NULL;
    if(newTDS->COMPONENT_TIMES){
        for (i = 0; i < F_INTERVAL; i++) {
            newTDS->COMPONENT_TIMES[i] = -1;
        }
    }
    newTDS->noValue = 1;
    newTDS->limitCondition = limitCondition;
    newTDS->currentCondEval = limitCondition?  0 : 1;

	return newTDS;						
}

void addTdsDependency(TDS* dependency, TDS* tds){
    if(!tds->linkedDependency && !tds->TOTAL_DEPENDENCIES_PT){
        // alocação inicial de primeiro no maximo 150 dependencias
        tds->linkedDependency = malloc(sizeof(TDS*) * MAX_DEPEND);
    }
    else{
        // DEVE REALIZAR REALLOC
        if(tds->TOTAL_DEPENDENCIES_PT + 1 > MAX_DEPEND ){
            TDS** newRef = realloc(tds->linkedDependency, (tds->TOTAL_DEPENDENCIES_PT + 1) * 2);
            if(!newRef) {
                fprintf(stderr, "[addTdsDependency] ERROR IN ALLOCATION \n");
                exit(-1);
            }
            tds->linkedDependency = newRef;
        }
    }
    tds->linkedDependency[tds->TOTAL_DEPENDENCIES_PT] = dependency;
    tds->TOTAL_DEPENDENCIES_PT++;
}

Object* getTdsValue(TDS* tds, int C_TIME){
    if(tds->delayed){
        return copyObject(tds->DATA_TIME[tds->LAST_DELAYED_ACCEPT_TIME]);
    }
    return copyObject(tds->DATA_TIME[C_TIME]);
}

void resolveDelayedTdsDependencies(TDS* tds, int C_TIME){
    // c_time não é o valor o qual a TDS começou a valer (só vale no próximo porque é delayed)
    if(C_TIME >= tds->LAST_DELAYED_ACCEPT_TIME){
        Object* dependency = tds->linkedDependency[0]->DATA_TIME[C_TIME-1]; // aceita o anterior com atraso
        if(!tds->DATA_TIME[tds->LAST_DELAYED_ACCEPT_TIME]){
            addDataToTds(tds,C_TIME,dependency);
            tds->LAST_DELAYED_ACCEPT_TIME = C_TIME;
        }
        else{
            // o input parou (podemos liberar a porta)
            if(!dependency){
                tds->LAST_DELAYED_ACCEPT_TIME = C_TIME; // vai referenciar um valor atual que é NULL
            }
            // SENÃO, ele mantem a referência anterior de ACCEPT ai ele indexa a partir desse (a ideia é tentar nao "copiar varios valores"
            // pode dar problema na hora de liberar memoria (ou gerar efeitos colaterais)?
        }
    }
}


void resolveMergerTdsDependencies(TDS *tds, int C_TIME){
    int i;
    Object* resolved = NULL;
    for (i = 0; i < tds->TOTAL_DEPENDENCIES_PT; i++) {
        TDS* dependency = tds->linkedDependency[i];
        // caso exista dado nesse instante (se não tiver não é necessário verificar pois não existe conflito de merge)
        if(dependency->DATA_TIME[C_TIME]){
            // se a dependencia já não foi resolvida anteriormente e não foi resolvida nessa passada atual
            if(!tds->DATA_TIME[C_TIME] && resolved == NULL){
                resolved = dependency->DATA_TIME[C_TIME];
            }
            else{
                // vale ressaltar que isso aqui é só preocaução (já que a atualização da tds dependente está agora na responsabilidade dela mesma)
                // elas que se "inscreveram" nas dependencias e ficam observando elas
                // deve anular para esse instante de tempo
                tds->DATA_TIME[C_TIME] = NULL;
                return;
            }
        }
    }
    // sempre pode ocorrer a possibilidade da TDS de input em questão também não possuir DADOS! e então nada deve ser feito
    if(resolved){
        addDataToTds(tds,C_TIME,resolved);
    }
}

int addDataToTds(TDS* currentTDS, int C_TIME, Object* value){
    int currentLimitCond = currentTDS->limitCondition? currentTDS->currentCondEval : 1;
    if(value && C_TIME >= currentTDS->I_INTERVAL && currentTDS->DATA_TIME[C_TIME] == NULL && currentLimitCond) {
        currentTDS->DATA_TIME[C_TIME] = value;
        if(currentTDS->noValue){
            currentTDS->noValue = 0;
        }
        return 1;
    }
    return 0;
}

void updateLimitCondition(TDS* tds, Object* condExpr){
    if(condExpr){
        if(condExpr->type != LOGICAL_ENTRY){
            fprintf(stderr,"Invalid filter expression used on %s. Specified condition was not a boolean type",tds->name);
            exit(-1);
        }
        else{
            int eval = *(int*)condExpr->values[0];
            tds->currentCondEval = eval;
            tds->currenCondBindRef = condExpr->SINTH_BIND;
        }
    }
}

void resetLimitConditionEval(TDS* tds){
    tds->currentCondEval = 0;
}

void letGoTDS(TDS* tds){

}

void printTdsValues(TDS* tds, int C_TIME){
    if(!tds){
        return;
    }

    if (!tds->delayed){
        if(tds->noValue || !tds->DATA_TIME[C_TIME] || tds->DATA_TIME[C_TIME]->type == NULL_ENTRY){
            printf("TDS(%s,%d) o----- NULL --------",tds->name,C_TIME);
        }
        else{
            Object* valueEncap = tds->DATA_TIME[C_TIME];
            if(valueEncap->type == NUMBER_ENTRY){
                printf("TDS(%s,%d) o----> value: %d --->---",tds->name,C_TIME,*(int*)valueEncap->values[0]);
            }
            else{
                printf("TDS(%s,%d) o----> value: %s --->---",tds->name,C_TIME,(char*)valueEncap->values[0]);
            }
        }
    }
    else{
        Object* valueEncap = tds->DATA_TIME[tds->LAST_DELAYED_ACCEPT_TIME];
        printf("-");
        if(tds->noValue || !valueEncap || valueEncap->type == NULL_ENTRY){
            printf("||-o TDS(%s,%d)",tds->name,C_TIME);
        }
        else{
            if(valueEncap->type == NUMBER_ENTRY){
                printf("|  %d  |-o TDS(%s,%d)",*(int*)valueEncap->values[0],tds->name,C_TIME);
            }
            else{
                printf("|  %s  |-o TDS(%s,%d)",(char*)valueEncap->values[0],tds->name,C_TIME);
            }
        }
    }
}

void prepareToPrintTDS(TDS* tds, int C_TIME){
    printTDS(tds,NULL,C_TIME);
    if(tds->limitCondition && tds->type == TDS_DEPEN){
        resetLimitConditionEval(tds);
    }
}

void printTDS(TDS* tds, TDS* dependent, int C_TIME){
    if (!tds){
        return;
    }
    if(tds->type == TDS_DEPEN){
        int i;
        for (i = 0; i < tds->TOTAL_DEPENDENCIES_PT; i++) {
            TDS* depRefFifo = NULL;
            if(tds->delayed){
                depRefFifo = tds;
            }
            printTDS(tds->linkedDependency[i],depRefFifo,C_TIME);
            if(!depRefFifo){
                printf("->");
            }
            if(tds->TOTAL_DEPENDENCIES_PT > 1){
                    if(i == (tds->TOTAL_DEPENDENCIES_PT/2)-1){
                        printf("\n");
                        printf("\t\t\t\t\t\t\t\t\t");
                        printf("o TDS(%s,%d)",tds->name,C_TIME);
                        if(dependent){
                            printTdsValues(dependent,C_TIME);
                        }
                        printf("\n");
                    }
                    else {
                        printf("\n");
                    }
            }
            else{
                if(tds->limitCondition){
                    if(tds->currentCondEval){
                        printf("o TDS(%s,%d)",tds->name,C_TIME);
                    }
                    else{
                        printf("|filter block|-o TDS (%s,%d)",tds->name,C_TIME);
                    }
                }
                else if (!tds->delayed){
                    printf("o TDS(%s,%d)",tds->name,C_TIME);
                }
                else{
                    if(tds->linkedDependency[i]->TOTAL_DEPENDENCIES_PT == 1){
                        printTdsValues(tds,C_TIME);
                    }
                }
            }
        }
    }
    else{
        printTdsValues(tds,C_TIME);
    }
}