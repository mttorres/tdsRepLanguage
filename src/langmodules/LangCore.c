//
// Created by mateus on 20/04/2021.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../../headers/LangCore.h"


/*
 * Funções Auxiliares Internas da linguagem
 */

STable* selectSMV_SCOPE(STable* scope, EnvController* controllerSmv){
    if(scope->type == FUNC || scope->childOfFunction){
        return accessSmvInfo(controllerSmv,FUNCTION_SMV,scope->indexRef);
//        return controllerSmv->functionsInfo[scope->indexRef];
    }
    else{
        return controllerSmv->mainInfo; // só vai retornar ports em casos de declarações de tds
    }
}

HeaderSmv * selectSMV_INFO(STable* scope, Object* functionPointer, EnvController* controllerSmv){
    if(!functionPointer){
        return controllerSmv->MAIN;
    }
    else{
//      objeto função (parametros vão ser adicionados a STABLE, e vamos ter o número esperado de parâmetros)
        // (vamos ter o index de referência do controller (header) e também o smvinfo
        int ALL_FUNCTION = *(int*) functionPointer->values[3];
//      int TDS_FUNCTION = *(int*) functionPointer->values[4];
        if(ALL_FUNCTION){
            return accessHeader(controllerSmv,MAIN,ALL_FUNCTION);
        }
//      if(TDS_FUNCTION){
//            return accessHeader(controllerSmv,PORTS,TDS_FUNCTION);
//     }
        fprintf(stderr, "FUNCTION SMV INFO NOT FOUND");
        exit(-1);
    }
}

// avaliar a importancia dos métodos acima, e se possível movimentar eles para o HeaderSmv.c ou para um Novo Controller.h


void checkTdsEvalReview(EnvController* controller){
    if(controller->interactiveMode){
        char awns[50];
        printf("\n\n");
        printf("Continue(y/n)?");
        scanf("%s", awns);
        if((awns[0] == 'n' || awns[0] == 'N') && awns[1] == '\0'){
            printf("Interpretation interrupted by user.\n");
            exit(0);
        }
        printf("\n\n\n");
        printf("---------------------------------------------------------\n\n\n");
    }
}

void resolveDependencies(TDS* currentTDS, STable* scope, EnvController* controllerSmv, int I_TIME, int C_TIME){

    Object* resolvedCondFilter = NULL;
    if(currentTDS->limitCondition){
        controllerSmv->filterContext = 1;
        controllerSmv->currentTDScontext = NULL;
        resolvedCondFilter = eval(currentTDS->limitCondition,scope,controllerSmv);
        if(C_TIME > I_TIME){
            updateAllAutomataFilter(resolvedCondFilter,controllerSmv);
        }
        updateLimitCondition(currentTDS, resolvedCondFilter);
        addDataToTds(currentTDS,C_TIME,currentTDS->linkedDependency[0]->DATA_TIME[C_TIME]);
    }
    else{
        if(currentTDS->delayed){
            resolveDelayedTdsDependencies(currentTDS,C_TIME);
        }
        else{
            resolveMergerTdsDependencies(currentTDS, C_TIME);
        }
    }
    int i;
    // sempre deve resolver as dependencias de uma TDS já que a embora a dependente possa não ter recebido valores diretamente esse instante
    // o "tipo" do type-set da outra pode ter mudado (nuXmv é bem rigido a respeito disso)
    for (i = 0; i < currentTDS->TOTAL_DEPENDENCIES_PT; i++) {
        propagateValueToTypeSet(currentTDS, currentTDS->linkedDependency[i],controllerSmv);
    }
    if(currentTDS->limitCondition){
        specTDS(currentTDS,NULL,C_TIME,I_TIME,controllerSmv,scope);
        letgoObject(resolvedCondFilter);
    }
}

void resolveLazyTdsSpec(STable *currentScope, EnvController *controllerSmv, int C_TIME, int I_TIME, Node *PROGRAM_PATH, TDS *currentTDS) {

    Object*  lazyValue = eval(PROGRAM_PATH, currentScope, controllerSmv);
    if(lazyValue->aList || lazyValue->type == LOGICAL_ENTRY){
        fprintf(stderr, "TDS VALIDATION ERROR: INCOMPATIBLE SPECIFICATION FOR TDS %s. DATA STRUCTURES ARE NOT ACCEPTED AS VALUES, ONLY SYMBOLIC VALUES.", currentTDS->name);
        exit(-1);
    }
    specTDS(currentTDS,lazyValue,C_TIME,I_TIME,controllerSmv,currentScope);
    addDataToTds(currentTDS,C_TIME,lazyValue);
}

void resolveTimeComponentSpec(STable *currentScope, EnvController *controllerSmv, int C_TIME, int I_TIME,
                              TDS *currentTDS, Object *timeComponent) {

    resolveLazyTdsSpec(currentScope, controllerSmv, C_TIME, I_TIME, timeComponent->values[1], currentTDS);
}

void resolveTdsLazyEvaluation(STable *currentScope, EnvController *controllerSmv, int C_TIME) {
    int I_TIME = *(int*) lookup(currentScope, "I_TIME")->val->values[0];
    int i;
    for (i = 0; i < controllerSmv->declaredPortsNumber; i++) {
        // resolve call-by-need cada expressão ativa da TDS.
        TDS* currentTDS = controllerSmv->declaredPorts[i];
        controllerSmv->currentTDScontext = currentTDS;
        if(C_TIME >=  currentTDS->I_INTERVAL){
            if(currentTDS->type == DATA_LIST){
                // eval de forma que ele deve saber qual componente temporal ele deve pegar
                if(currentTDS->COMPONENT_TIMES[C_TIME] != -1){
                    Object* tdsDataSpec = currentTDS->DATA_SPEC;
                    Object* timeComponent = (Object*) tdsDataSpec->values[currentTDS->COMPONENT_TIMES[C_TIME]];
                    resolveTimeComponentSpec(currentScope,controllerSmv,C_TIME,I_TIME,currentTDS,timeComponent);
                }
            }else{
                // tds_dependentes já são resolvidas sempre (tds desse tipo deveria ser adicionada aos lazy?)
                if(currentTDS->type != TDS_DEPEN){
                    resolveLazyTdsSpec(currentScope, controllerSmv, C_TIME, I_TIME, currentTDS->DATA_SPEC, currentTDS);
                }
                else{
                    // senão deve verificar a dependência dessa TDS para atribuir o DATA_TIME também e resolver seu type-set
                    resolveDependencies(currentTDS,currentScope,controllerSmv,I_TIME,C_TIME);
                }

            }
        }
        if(controllerSmv->interactiveMode){
            prepareToPrintTDS(currentTDS,C_TIME);
            printf("\n\n");
        }
        else{
            resetLimitConditionEval(currentTDS);
        }
    }
    checkTdsEvalReview(controllerSmv);
    controllerSmv->currentTDScontext = NULL;
}

// CASO FORA DE FLUXO 1: e se ele "pular", ex: commitar c_time = 2 (quando era 0 antes), ele pulou o 1! A gente deve ver a "diferença"
// CHAMAR ESSA FUNÇÃO DIFERENÇA VEZES.
// CASO FORA DE FLUXO 2: deve chamar isso também ao FIM DO PROGRAMA (para cada CHAMADA RESTANTE, ex: C_TIME terminou em 4, deve chamar até F_TIME (5, ... , F_TIME).
// DIFERENÇA VEZES DE NOVO!
void commitCurrentTime(STable* currentScope, EnvController* controllerSmv, int changedTo){
    // deve resolver a avaliação para cada TDS "n" vezes. Antes do proximo intervalo ou fim do programa.
    int i;
    int C_TIME = *(int*) lookup(currentScope,"C_TIME")->val->values[0];
    for (i = C_TIME; i < changedTo; i++) {
        if(controllerSmv->interactiveMode){
            printf("--------------TDS EVALUATION (time = %d)------------------\n\n\n",i);
        }
        resolveTdsLazyEvaluation(currentScope, controllerSmv, i);
        int next = i+1;
        if(next != changedTo){
            void* vp[] = {&next};
            updateValue("C_TIME", vp, T_DIRECTIVE_ENTRY, 1, -1, -1, currentScope, 0);
        }
    }
}


Object * notObjectOperation(Object* o){
    if(o->aList || (o->type != LOGICAL_ENTRY))
    {
        fprintf(stderr, "INCOMPATIBLE OPERAND %s FOR THE (not) OPERATION!",o->SINTH_BIND);
        exit(-1);
    }
    int r;
    r =  !(*(int*) o->values[0]);
    void* rp[] = {&r};
    char resultingBind[strlen(o->SINTH_BIND)+2];
    createExprBind(resultingBind, o, NULL, "!");
    letgoObject(o);
    return createObject(LOGICAL_ENTRY, 1, rp, -1, resultingBind, NULL);
}

int comparator(Object* o1, Object* o2, char* op){

    if(!o1->aList && !o2->aList && o1->type == NUMBER_ENTRY && o2->type == NUMBER_ENTRY)
    {
        return (*(int*)o1->values[0]) == (*(int*)o2->values[0]);
    }
    if(!o1->aList && !o2->aList && o1->type == LABEL_ENTRY && o2->type == LABEL_ENTRY)
    {
        char* str1 = (char*) o1->values[0];
        char* str2 = (char*) o2->values[0];
        return strcmp(str1,str2);
    }
    fprintf(stderr, "INCOMPATIBLE OPERANDS %s and %s FOR THE (%s) OPERATION!",o1->SINTH_BIND,o2->SINTH_BIND,op);
    exit(-1);
}

_Bool IMP(_Bool a, _Bool b){ return !a || b; }


_Bool BIMP(_Bool a, _Bool b){ return (a && b) || (!a && !b); }

void updateVariable(char* varName, Object *var, Object *expr, STable* scope, int index, int C_TIME) {

    int prevTimeContext = var->timeContext;
    if(prevTimeContext == C_TIME){
        fprintf(stderr, "ASSIGN ERROR: redefinition of %s in same time interval \n", varName);
        exit(-1);
    }
    /*
     * Caso var não seja NULL (isto é "placeholder")
     * Realiza operação normal, senão trata o null
     */
    if(var->type == NULL_ENTRY){
        if(expr->type != NULL_ENTRY) {
            updateNullRef(var,expr);
            updateValue(varName, expr->values, expr->type, expr->OBJECT_SIZE, index, -1, scope, C_TIME);
        }
        // senao nao faz nada x = NULL , atribuir novamente a null não muda nada
    }
    else{
        // VALIDAÇÃO DE TIPOS
        if(var->type != expr->type){
            fprintf(stderr, "ASSIGN ERROR: incompatible type for  %s \n", varName);
            exit(-1);
        }
        updateValue(varName, expr->values, expr->type, expr->OBJECT_SIZE, index, -1, scope, C_TIME);
    }
}

void computeTDSDependentOperations(Node*n, char* portName, STable* scope, TDS* newTDS, EnvController* controller, int I_TIME, int C_TIME){
    Object * dependenceList = eval(n->children[0],scope,controller);
    Object * DEP_HEAD = dependenceList->OBJECT_SIZE > 1 ? dependenceList->values[0] : NULL;
    if(DEP_HEAD && DEP_HEAD->type == TDS_ENTRY || dependenceList->type == TDS_ENTRY) {
        if (dependenceList->OBJECT_SIZE > 2) {
            fprintf(stderr,
                    "[WARNING] %s definition uses more than 2 inputs.\nIt is recommended to check the input model, for instance, reo2nuXmv only generates merge-like connections with at most 2 inputs\n",
                    portName);
            exit(-1); // enquanto o modelo do daniel não é estendido para mais de 2 portas para o merger
        }
        if(( newTDS->delayed || dependenceList->OBJECT_SIZE > 1) && C_TIME > I_TIME){
            fprintf(stderr, "[WARNING] %s uses a temporal condition to initialization. It is recommended to review your specification.\nThis behaviour is usually acceptable in case the modeled port is lossy (eg: LossySync)\n",
                    portName);
        }

    }
    else{
        fprintf(stderr, "ERROR: BAD USE OF TDS DEFINITION, %s is not receiving input from another TDS! \n",portName);
        exit(-1);
    }

    if(dependenceList->OBJECT_SIZE > 1){
        int i;
        for (i = 0; i < dependenceList->OBJECT_SIZE; i++) {
            Object* ENCAPSULATED_DEPENDENCY = (Object*) dependenceList->values[i];
            TDS* DEPENDENCY = (TDS*) ENCAPSULATED_DEPENDENCY->values[0];
            if(DEPENDENCY){
                addTdsDependency(DEPENDENCY,newTDS);
            }
        }
    }
    else{
        addTdsDependency(dependenceList->values[0], newTDS);
    }
    controller->IO_RELATION = 1;
}

Object *computeTDSBasicOperations(Node *pathForDepen, char *portName, TDS_TYPE type, Object *tdsSpec, int delayed,
                                  STable *scope, Node *pathForCond, EnvController *controller) {
    int C_TIME = *(int*) lookup(scope,"C_TIME")->val->values[0];
    int I_TIME = *(int*)lookup(scope,"I_TIME")->val->values[0];
    int F_TIME  = *(int*) lookup(scope,"F_TIME")->val->values[0];

    TDS* newTDS = createTDS(portName, type, tdsSpec, delayed, C_TIME, F_TIME, pathForCond);
    if(newTDS->limitCondition){
        controller->filterUsed = 1;
    }
    if(type == TDS_DEPEN){
        computeTDSDependentOperations(pathForDepen,portName,scope,newTDS,controller,I_TIME,C_TIME);
    }

    void* vp[] = {newTDS};
    char* TDS_BIND = createReferenceTDS(portName);
    Object* encapsulatedTDS = createObjectDS(TDS_ENTRY,1,vp,C_TIME,TDS_BIND,0);
    free(TDS_BIND); // pode parecer "irrelevante" mas é uma garantia, o createObject não cuida do free dos binds. Em especial por causa da cópia de variáveis.
    preProcessTDS(encapsulatedTDS,controller,C_TIME,I_TIME,F_TIME);
    return encapsulatedTDS;
}

