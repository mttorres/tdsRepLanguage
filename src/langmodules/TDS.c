#include <stdlib.h>
#include <stdio.h>
#include "../../headers/TDS.h"


TDS* createTDS(char *name, TDS_TYPE type, Object *valueList, int delayed, char *functionRef, int I_INTERVAL, int F_INTERVAL, Object *limitCondition) {

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
	newTDS->functionRef = functionRef;
    newTDS->SMV_REF = -1;

    newTDS->COMPONENT_TIMES = type == DATA_LIST?  malloc(sizeof(int)*F_INTERVAL+1) : NULL;
    if(newTDS->COMPONENT_TIMES){
        for (i = 0; i < F_INTERVAL; i++) {
            newTDS->COMPONENT_TIMES[i] = -1;
        }
    }

    newTDS->noValue = 1;
    newTDS->limitCondition = limitCondition;

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
    int currentLimitCond = currentTDS->limitCondition? *(int*) currentTDS->limitCondition->values[0]: 1;
    if(value && value->type != NULL_ENTRY && currentTDS->DATA_TIME[C_TIME] == NULL && currentLimitCond) {
        currentTDS->DATA_TIME[C_TIME] = value;
        if(currentTDS->noValue){
            currentTDS->noValue = 0;
        }
        return 1;
    }
    return 0;
}

void* letGoTDS(TDS* tds){

}