#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <TDS.h>
#include "../headers/PostProcess.h"


typedef enum MAP_CONVERSIONS { ANY, ANY_TERM, ANY_BREAK_LINE, UN_OP, OP, REDEF_NAME, NAME_BY_SCOPE, NAME_SSCOPE,
                               INIT, NEXT, ASSIGN_TO,
                               ASSIGN_TO_TAB_BREAK_LINE, CASE, CASE_EVAL, N_CASE_EVAL, DEFAULT_CASE_EVAL, EQUAL_CASE_EVAL,
                               INTERVAL_DEC, BOOLEAN_DEC ,SET, V_MODULE_DEC ,PAR, TDS_MODULE_NAME } MAP_CONVERSIONS;

                                                // ex: 1 + 1
char* SmvConversions[] = {"%s", "%s;",  "%s\n", "%s%s", "%s %s %s ", "%s_redef%d%", "%s_scope%d_%d","%s_scope%d_%d_%d_%d",
                          "init(%s)", "next(%s)", "%s:= %s;",
                          "\t%s:= %s;\n",  "case \n\t\t%s\n\t\tTRUE : %s; \n\tesac",
                          "%s : %s;", "\n\t\t%s : %s;\n", "TRUE : %s; \n", "%s = %s : %s; \n",
                          "\t%s : %d..%d;\n", "\t%s : boolean;\n" , "\t%s : {%s};\n", "\t%s : %s;\n", "%s, %s", "tds_%s" };

int  ALOC_SIZE_LINE = 300;
int  DEFAULT_CASE_SIZE = 30; // 29 +1


void copyValueBind(Object* o, char* bind,int index,int defaultValue)
{
    char* formatS = "%s";
    char* formatN = "%d";
    char* formatRef = "%s[%d]";

    if(!index) {
        if (o->SINTH_BIND) {
            sprintf(bind, formatS, o->SINTH_BIND);
        } else {
            if (o->type == NUMBER_ENTRY || o->type == T_DIRECTIVE_ENTRY) {
                    sprintf(bind, formatN, defaultValue? 0 : *(int *) o->values[0]);
            }
            if (o->type == LOGICAL_ENTRY) {
                sprintf(bind, formatS, *(int *) o->values[0] && !defaultValue ? "TRUE" : "FALSE");
            }
            if (o->type == LABEL_ENTRY || o->type == NULL_ENTRY) {
                sprintf(bind, formatS, defaultValue? "NULL" : (char *) o->values[0]);
            }
            if (o->type == TDS_ENTRY) {
                // ...
            }
        }
    }
    else{
        // listas vão ser objetos (de tamanho > 1) que guardam objetos
        Object* listComponent = (Object*) o->values[index];
        copyValueBind(o,bind,0,defaultValue);
    }
}

void createExprBind(char *result, Object *o1, Object *o2, char *op) {
    if(!o2){
        sprintf(result,SmvConversions[UN_OP],op,o1->SINTH_BIND);
    }
    else{
        sprintf(result,SmvConversions[OP],o1->SINTH_BIND,o2->SINTH_BIND);
    }
}



char *createConditionCube(char *opBind1, char *opBind2, char *operation, char *evaluation, int firstCond)
{
        char inter[ALOC_SIZE_LINE];
        char* interPt = inter;
        char* cube = malloc(ALOC_SIZE_LINE);

        // gera cond ( bind1 op bind2) ou opbind1
        if(opBind2[0] != '\0'){
                sprintf(inter,SmvConversions[OP],opBind1,operation,opBind2);
        }
        else{
                sprintf(inter,SmvConversions[ANY],opBind1);
        }
        if(evaluation){
            int indexConversion = firstCond? CASE_EVAL : N_CASE_EVAL;
            sprintf(cube,SmvConversions[indexConversion],inter,evaluation);
        }
        else{
            sprintf(cube,SmvConversions[ANY],inter);
        }
        return cube;
}


void bindCondition(STable* scope, Object* conditionExpr){
    if(scope->type != IF_BLOCK && scope->type != ELSE_BLOCK){
        fprintf(stderr, "[bindCondition]: %d scope cannot be binded to %s! \n",scope->type,conditionExpr->SINTH_BIND);
        exit(-1);
    }
    if(!scope->conditionBind){
        // usa condição
        if(scope->parent->type != GLOBAL){
            STable * parent = scope->parent;
            char* childCond = conditionExpr->SINTH_BIND; // atual
            char* parentCond = parent->conditionBind; // primeiro pai
            char* interResult = NULL;
            char* auxChildCond = NULL;
            while(parent && parent->type != GLOBAL){
                interResult = createConditionCube(parentCond,childCond,"&",NULL,1);
                parent = parent->parent;
                if(parent){
                    parentCond = parent->conditionBind; // proximo pai
                }
                if(auxChildCond){
                    free(auxChildCond);
                }
                childCond = interResult; // o filho é o cubo atual
                auxChildCond = childCond; // temos uma referência para ser liberada na proxima passada
            }
            scope->conditionBind = malloc(sizeof(char)*(strlen(interResult)+1));
            strcpy(scope->conditionBind,interResult);
            free(interResult);
        }
        // usa a condição da expressão
        else{
            scope->conditionBind = malloc(sizeof(char)*(strlen(conditionExpr->SINTH_BIND)+1));
            strcpy(scope->conditionBind,conditionExpr->SINTH_BIND);
        }
    }
}



char* formatDirective(int ctime){
    char* directiveValueBind = malloc(sizeof(char)*ALOC_SIZE_LINE/10);
    sprintf(directiveValueBind, "%d", ctime);
    return directiveValueBind;
}

char* formatCondtion(STable* scope, int ignoreCond, int ignoreTemporal, char* valueBind, char* directiveValueBind, int firstCondition){
    char* condition = !ignoreCond &&  (scope->type == IF_BLOCK || scope->type == ELSE_BLOCK) ? scope->conditionBind : NULL; // SINTH_BIND do escopo
    // SINTH_BIND da condição temporal, ex: "next(time) = 2"
    char* temporalCondition = directiveValueBind && !ignoreTemporal?
                              createConditionCube("next(time)", directiveValueBind, "=", NULL, 1)
                                                              : NULL;
    // ponteiro auxiliar que referencia a condição temporal criada
    char* auxReftemporalCondition = temporalCondition;

    // se existe alguma condição vinda do escopo e uma condição temporal, as concatena, senão usa apenas uma delas ou nenhuma delas
    char* conditionCube = temporalCondition && condition ?
                          createConditionCube(auxReftemporalCondition, condition, "&", valueBind, firstCondition) :
                          auxReftemporalCondition ? createConditionCube(auxReftemporalCondition, "", "", valueBind,
                                                                        firstCondition) :
                          condition ? createConditionCube(condition, "", "", valueBind, firstCondition) : NULL;

    // libera o cubo sem avaliação utilizado anteriormente para o passo anterior
    if(temporalCondition){
        free(temporalCondition);
    }
    return conditionCube;
}


char *formatBinds(int ctime, int changeContext, char *directiveValueBind, char *valueBind, char *defaultValueBind,
                  Object *expr, STable *scope, int firstCondition, int initVar, int ignoreTemporal, int ignoreCond) {

    sprintf(directiveValueBind, "%d", ctime); // SINTH_BIND da diretiva temporal corrente
    copyValueBind(expr,valueBind,0,0); // SINTH_BIND da expressão, pode variar para vetores e estruturas complexas

    char* condition = !ignoreCond &&  (scope->type == IF_BLOCK || scope->type == ELSE_BLOCK) ? scope->conditionBind : NULL; // SINTH_BIND do escopo
    // SINTH_BIND da condição temporal, ex: "next(time) = 2"
    char* temporalCondition = changeContext && !ignoreTemporal?
                              createConditionCube("next(time)", directiveValueBind, "=", NULL, 1)
                                           : NULL;

    // ponteiro auxiliar que referencia a condição temporal criada
    char* auxReftemporalCondition = temporalCondition;

    // se existe alguma condição vinda do escopo e uma condição temporal, as concatena, senão usa apenas uma delas ou nenhuma delas
    char* conditionCube = temporalCondition && condition ?
                          createConditionCube(auxReftemporalCondition, condition, "&", valueBind, firstCondition) :
                          auxReftemporalCondition ? createConditionCube(auxReftemporalCondition, "", "", valueBind,
                                                                        firstCondition) :
                          condition ? createConditionCube(condition, "", "", valueBind, firstCondition) : NULL;

    // libera o cubo sem avaliação utilizado anteriormente para o passo anterior
    if(temporalCondition){
        free(temporalCondition);
    }

    // "otimização" para criar o caso default, se necessário
    if(initVar){
        copyValueBind(expr,defaultValueBind,0,1);
    }
    // o default vai ser ele mesmo (usado depois na criação de expressão)
    else{
        // devemos usar o próprio nome da variável (caso já exista) (mais especificamente... o nome "anterior" dessa variável
        // mas primeiro devemos corrigir o segfault atual senão continuaremos tendo problemas
    }

    return conditionCube;
}


void createType(char *varName, HeaderSmv *header, STable *writeSmvTypeTable, char *newValueBind, Object *newValue,
                int type)
{
    char* newType = malloc(sizeof(char)*ALOC_SIZE_LINE);
    int pos = header->VAR_POINTER;
    if(type == NUMBER_ENTRY || type == T_DIRECTIVE_ENTRY){
        int valSin = newValue? *(int*) newValue->values[0] : 0;
        int min = 0;
        int max = 1;
        if(valSin <= 0){
            min = valSin;
            sprintf(newType, SmvConversions[INTERVAL_DEC], varName, valSin, max);
        }
        if(valSin > 0){
            max = valSin;
            sprintf(newType, SmvConversions[INTERVAL_DEC], varName, min, valSin);
        }
        char* auxDelim = strstr(newType,":");
        char* auxFim = strstr(auxDelim,"..");
        int pointIni = (auxDelim-newType+2);
        int pointEnd = ((auxFim-newType))-1;
        int tam = strlen(newType);

        void* po[] = {&pos, &tam, &pointIni, &pointEnd, &min, &max};
        addValue(varName, po, WRITE_SMV_INFO, 6, 0, writeSmvTypeTable, 0);
    }
    else if(type == LOGICAL_ENTRY){
        sprintf(newType, SmvConversions[BOOLEAN_DEC], varName, newValueBind, newValueBind);
        int tam = strlen(newType);
        void* po[] = {&pos, &tam};
        addValue(varName, po, WRITE_SMV_INFO, 2, 0, writeSmvTypeTable, 0);

    }
    else if(type == TDS_ENTRY || type == LABEL_ENTRY)
    {
        sprintf(newType, SmvConversions[SET], varName, newValueBind);
        int tam = strlen(newType);
        int* typeSetHashMap = malloc(sizeof(int)*MAX_SIMPLE);
        if(type == TDS_ENTRY){
            int hashNULL = hash("NULL",NULL);
            int hashZERO = hash("0",NULL);
            int hashONE = hash("1",NULL);
            typeSetHashMap[hashNULL] = 1;
            typeSetHashMap[hashZERO] = 1;
            typeSetHashMap[hashONE] = 1;
        }
        else{
            typeSetHashMap[hash(newValueBind,NULL)] = 1;
        }
        void* po[] = {&pos, &tam,typeSetHashMap};
        addValue(varName, po, TYPE_SET, 3, 0, writeSmvTypeTable, 0);

        //addTypeSetSmv(varName,po,TYPE_SET,2,);
    }
    else{
        sprintf(newType,SmvConversions[V_MODULE_DEC],varName,newValueBind);
        int tam = strlen(newType);
        void* po[] = {&pos, &tam};
        addValue(varName, po, WRITE_SMV_INFO, 2, 0, writeSmvTypeTable, 0);
    }
    header->varBuffer[header->VAR_POINTER] = newType;
    header->VAR_POINTER += 1;
}

void updateType(char *varName, HeaderSmv *header, STable *writeSmvTypeTable, const char *newValue, int type, int minmax,
                Object *newValueNumber)
{
    // começando com numérico x..y;
    // criar enum mapeador ao decorrer...
    if(minmax != -1 && type == NUMBER_ENTRY || type == T_DIRECTIVE_ENTRY)
    {
        int pos;
        int size;
        int pointIni;
        int pointEnd;
        // recuperar da tabela
        TableEntry* entryPosType;
        entryPosType =  lookup(writeSmvTypeTable,varName);
        if(entryPosType){
            pos = *(int*) entryPosType->val->values[0];
            size = *(int*) entryPosType->val->values[1];
            pointIni = *(int*) entryPosType->val->values[2];
            pointEnd = *(int*) entryPosType->val->values[3];

            int newPointIni = 0;
            int newPointEnd = 0;
            int sizeNew = strlen(newValue);
            // min..max;
            if(minmax)
            {
                pointIni = pointEnd+3; // n..max;
                // nota! o size já está indexbased!
                pointEnd = header->varBuffer[pos][size-1] == '\n' ? size-3  : size-2; // max;\n (-1 do index based) - (-2 ou -1 dependendo do fim)
                //size = header->varBuffer[pos][size-1] == '\n' ? size-1 : size;
            }
            updateSubStringInterval(newValue, header->varBuffer[pos], sizeNew, pointIni, pointEnd, size, &newPointIni,
                                    &newPointEnd, 0);
            size = -1*((pointEnd-pointIni+1) - sizeNew) + size;
            void* vpSize[] = {&size};
            updateValue(varName, vpSize, WRITE_SMV_INFO, 1, 1, -1, writeSmvTypeTable, 0);

            // atualizar o fim do intervalo não mudar a nossa variável pointEnd também! Só atualiza o tamanho
            if(!minmax)
            {
                void* vpInEnd[] = {&newPointEnd};
                updateValue(varName, vpInEnd, WRITE_SMV_INFO, 1, 3, -1, writeSmvTypeTable, 0);
                int min = newValueNumber? *(int*) newValueNumber->values[0] : 0;
                void* vpmin[] = {&min};
                updateValue(varName, vpmin, WRITE_SMV_INFO, 1, 4, -1, writeSmvTypeTable, 0);
            }
            else{
                int max = newValueNumber? *(int*) newValueNumber->values[0] : 1;
                void* vpmax[] = {&max};
                updateValue(varName, vpmax, WRITE_SMV_INFO, 1, 5, -1, writeSmvTypeTable, 0);
            }
        }
        else{
            printf("[updateType] WARNING: type of %s not declared on headers \n",varName);
        }
    }
    if(type == LOGICAL_ENTRY){
        // não faz nada
    }
}
// quebrar em spec next e spec init
void createAssign(char *varName, HeaderSmv *header, STable *writeSmvTypeTable, const char *newValue, char *condition,
                  int typeExpr, char *defaultEvalCond)
{

    printf("...\n");
    char* exprResultString = malloc(sizeof(char)*ALOC_SIZE_LINE);
    char exprInterL[300];
    char exprInterR[300];
    sprintf(exprInterL, SmvConversions[typeExpr], varName);
    // parâmetros a ser salvo na tabela auxiliar SMV
    int size;
    int pointIni;
    int pointEnd;

    if(condition)
    {
        if(typeExpr == INIT){
            sprintf(exprInterR,SmvConversions[CASE],condition,defaultEvalCond); // default
        }
        else{
            if(defaultEvalCond){
                sprintf(exprInterR,SmvConversions[CASE],condition,defaultEvalCond); // default sendo uma outra referência
            }
            else{
                sprintf(exprInterR,SmvConversions[CASE],condition,varName); // default sendo o inicial
            }
        }
        free(condition);
        sprintf(exprResultString,SmvConversions[ASSIGN_TO_TAB_BREAK_LINE],exprInterL,exprInterR);

        char* auxChPoint;
        auxChPoint = strstr(exprResultString,";\n"); // devolve exatamente o ponto, devemos ir para o \n
        long dif = auxChPoint-exprResultString;
        size = strlen(exprResultString);
        pointIni = dif+1;
        pointEnd = pointIni;
    }
    else{
        // atualiza o init/next dessa função
        // criar a expressão definitiva com sprintf?
        // pros : é facil
        // cons: não tenho controle sobre qual parte da string minha expressão começa ou termina...
        // NA VERDADE EU TENHO, posIni:  até o = (posso salvar em um array mapeador também)
        //  posEnd = posIni + tamanhoString(expressão)

        sprintf(exprInterR,SmvConversions[ANY],newValue);
        sprintf(exprResultString,SmvConversions[ASSIGN_TO_TAB_BREAK_LINE],exprInterL,exprInterR);

        size = strlen(exprResultString);
        pointIni = (strlen(exprInterL) - 1) + 5; //  %s:= %s (+2 para pular ele mesmo e o : e depois = e espaço, e fora o \t no inicio)
        pointEnd = pointIni + strlen(exprInterR)-1;

    }
    // escreve atribuição no buffer
    // encapsular em método(!)
    header->assignBuffer[header->ASSIGN_POINTER] = exprResultString;
    int pos = header->ASSIGN_POINTER;
    header->ASSIGN_POINTER += 1;

    //atualiza tabela auxiliar para init/next(var) (se necessário)
    if(writeSmvTypeTable) {
        void* po[] = {&pos, &size,&pointIni,&pointEnd};
        addValue(exprInterL, po, WRITE_SMV_INFO, 4, 0, writeSmvTypeTable, 0);
    }

}





void updateAssign(char* varName ,HeaderSmv* header, STable* writeSmvTypeTable, char* newValue, char* condition, int type ,int typeExpr, int minmax)
{
    // tratamento de init/next(varName):= case ... TRUE : x; esac; , geralmente TRUE: NULL ou outra condição parecida
    // é sempre o "delmitador final", vai ser um caso similar ao anterior porém entre ponto de interesse - condição default, já que condições não mudam!
    // o que muda é que vamos er que concatenar esse default ? (NÃO, é criado naturalmente)
    // ou seja devemos pegar o que está sendo adicionado a sizenew e jogar uma condição se (ela existir)
    if(condition)
    {
        newValue = condition;
    }
    char *updated = NULL; // ponteiro para referenciar a string que vamos tratar
    char upVar[300]; // ponteiro para localizar o assign na tabela
    sprintf(upVar,SmvConversions[typeExpr],varName); // recupera string default init/next
    int sizeNew = strlen(newValue);

    int pos;
    int size;
    int pointIni;
    int pointEnd;

    TableEntry* entryPosSizeAssing;
    entryPosSizeAssing =  lookup(writeSmvTypeTable,upVar);

    // recupera a posição no buffer, o tamanho da string, e o intervalo de interesse [i,f]
    pos = *(int*) entryPosSizeAssing->val->values[0];
    size = *(int*) entryPosSizeAssing->val->values[1];
    pointIni = *(int*) entryPosSizeAssing->val->values[2];
    pointEnd = *(int*) entryPosSizeAssing->val->values[3];

    int newPointInit = 0;
    int newPointEnd = 0;

    // verifica o tamanho possível após a mudança (o +1 é justamente porque se o intervalo é x-y = 0, o tamanho é 1,
    // ou seja torna como size based ao inves de index based)
    if((-1*((pointEnd-pointIni+1) - sizeNew) + size)  >= ALOC_SIZE_LINE)
    {
        char* newStrSize = realloc(header->assignBuffer[pos],ALOC_SIZE_LINE*2);
        ALOC_SIZE_LINE = ALOC_SIZE_LINE*2;
        if(newStrSize == NULL)
        {
            fprintf(stderr, "FAIL IN REALLOCATE HEADER SIZE FOR %s !",upVar);
            exit(-1);
        }
        header->assignBuffer[pos] = newStrSize;
    }
    updated = header->assignBuffer[pos];

    // é um simples next(x)/init(x):= yyyyyyy;  -> substituir y's por expressão nova em newValue antes do delmitador (;)
    // os delmitadores podem variar, ex: em caso de next(time) o delimitador é (:)
    updateSubStringInterval(newValue, updated, sizeNew, pointIni, pointEnd, size, &newPointInit, &newPointEnd, 1);

    // atualizar range de interesse e tamanho da string na tabela!
    // fazer duas chamadas por enquanto
    size = -1*((pointEnd-pointIni+1) - sizeNew) + size;
    void* vpSize[] = {&size};  // evita ter que "reescrever" o vetor inteiro separando em varios vps
    // é como se a gente tivesse atualizando cada indice dos vetores menos o (0)
    updateValue(upVar, vpSize, WRITE_SMV_INFO, 1, 1, -1, writeSmvTypeTable, 0);
    void* vpIni[] = {&newPointInit};
    updateValue(upVar, vpIni, WRITE_SMV_INFO, 1, 2, -1, writeSmvTypeTable, 0);
    void* vpInEnd[] = {&newPointEnd};
    updateValue(upVar, vpInEnd, WRITE_SMV_INFO, 1, 3, -1, writeSmvTypeTable, 0);

    if(condition){
        free(condition);
    }

}

char *processActiveName(STable *currentScope, char *varName, int notExistsOutScope, int isOnNextContext, int type) {

    char* useVar = NULL;
    if(type != TDS_ENTRY) {
        char interScope[ALOC_SIZE_LINE];  //nome com info de scope
        char interRedef[ALOC_SIZE_LINE];  //nome com redefinição
        // qualquer escopo diferente de GLOBAL/MAIN
        if (notExistsOutScope && (currentScope->order || currentScope->level)) {
            // if, else, fors ....
            if (currentScope->parent->type != GLOBAL) {
                sprintf(interScope, SmvConversions[NAME_SSCOPE], varName, currentScope->parent->level,
                        currentScope->parent->order, currentScope->level, currentScope->order);
            } else {
                sprintf(interScope, SmvConversions[NAME_BY_SCOPE], varName, currentScope->level, currentScope->order);
            }
            useVar = interScope;
        } else {
            useVar = varName;
        }
        if (isOnNextContext) {
            sprintf(interRedef, SmvConversions[NEXT], useVar);
            useVar = interRedef;
        }
    }
    else{
       char interTDS[ALOC_SIZE_LINE];
        sprintf(interTDS, SmvConversions[TDS_MODULE_NAME], varName);
        useVar = interTDS;
    }
    char* activeName = malloc(sizeof(ALOC_SIZE_LINE));
    strcpy(activeName,useVar);
    return activeName;
}

char *formatValueBind(char *varName, STable *parentScope, Object *expr, int index, int isDefault, int isSelf) {
    if(isSelf){
        char* useVar = processActiveName(parentScope, varName, 1, 0, expr->type);
        return useVar;
    }
    else{
        char* valueBind = malloc(sizeof(char)*ALOC_SIZE_LINE);
        copyValueBind(expr,valueBind,index,isDefault);
        return valueBind;
    }
}

Object* refCopyOfVariable(TableEntry* var){
    char* useVar = NULL;
    // temos que usar escopo de VAR não o escopo atual de onde a chamada ocorre!
    // como nesse caso é necessário referênciar EXATAMENTE o nome da variável,
    useVar = processActiveName(var->parentScope, var->name, 1, var->val->timeContext, var->val->type);
    Object* copyRef = copyObject(var->val);
    if(useVar){
        free(copyRef->SINTH_BIND);
//      copyRef->SINTH_BIND[0] = '\0';
        copyRef->SINTH_BIND =  malloc(sizeof(char)*(strlen(useVar)+ 1));
        strcpy(copyRef->SINTH_BIND,useVar);
    }
    free(useVar);
    return copyRef;
}

void specAssign(int varInit, char *varName, int contextChange, HeaderSmv *header, STable *scope, STable *writeSmvTypeTable,
           Object *newValue, int redef, int typeExpr, int C_TIME)
{
    // strings para binds
    //binds da expressão
    char* newValueBind = NULL; //vai ser usado para o valor passado para o assign
    char* directiveValueBind = NULL; //vai ser usado para o tempo corrente
    char* defaultValueBind = NULL; //valor default para expressões next que necessitem inicialização (ex: init = 0, para inteiros)
    char* conditionCube = NULL ; // cubo de condições (mudança de contexto ou escopo if/else)

    // decide o nome apropriado para a variável
    char* useVar = NULL; // por default, usamos o nome da varável (se não for, em escopos diferentes ou ainda em redef )
    useVar = processActiveName(scope, varName, varInit, 0, newValue->type);

    int minmax = -1;
    if(typeExpr  && newValue->type == NUMBER_ENTRY) {
        TableEntry* info = lookup(writeSmvTypeTable,useVar);
        if(info){
            int min = *(int*) info->val->values[4];
            int max = *(int*) info->val->values[5];
            int new = *(int *) newValue->values[0];

            minmax = new < min ? 0 :
                     new > max ? 1 : minmax;
        }
    }
    int defaultSelf = !varInit && scope->type == IF_BLOCK || scope->type == ELSE_BLOCK;
    defaultValueBind = formatValueBind(varName, scope, newValue, 0, 1, defaultSelf);
    newValueBind = formatValueBind(varName, scope, newValue, 0, 0, 0);

    // next
    if(typeExpr){
        // criar next(useVar)
        char statevarname[ALOC_SIZE_LINE];
        sprintf(statevarname,SmvConversions[NEXT],useVar);
        //verifica se existe next(statevarname)
        directiveValueBind = formatDirective(C_TIME);
        //defaultValueBind = formatValueBind(newValue,0,1); // vai ser só para o caso de ref de uma variável que foi atualizada dentro de um if (já existe fora do escopo atual)

        if(lookup(writeSmvTypeTable,statevarname)){
            conditionCube = formatCondtion(scope,0,0,newValueBind,directiveValueBind,0);
            updateAssign(useVar, header, writeSmvTypeTable, newValueBind, conditionCube, newValue->type, NEXT, minmax);
        }
        else{
            conditionCube = formatCondtion(scope,0,0,newValueBind,directiveValueBind,1);
            createAssign(useVar, header, writeSmvTypeTable, newValueBind, conditionCube, NEXT, NULL);
        }
        Object* auxRefValue = newValue->type == NUMBER_ENTRY? newValue : NULL;
        updateType(useVar, header, writeSmvTypeTable, newValueBind, newValue->type, minmax, auxRefValue);
        free(directiveValueBind);
    }
    // init casos
    // caso 1 :  init default + next ( seja de variável que só existe em t>0(pode também ser uma redefinição) (devem ignorar condições nesse init)
    // caso 2 : init unico, seja para variáveis que foram redefinidas em t = 0 ou declarações comuns
    else{
        if((C_TIME && redef) || contextChange){

            conditionCube = formatCondtion(scope,1,1,newValueBind,directiveValueBind,1);

            createType(useVar, header, writeSmvTypeTable, defaultValueBind, newValue, newValue->type);
            createAssign(useVar, header, writeSmvTypeTable, defaultValueBind, conditionCube, INIT, defaultValueBind);
        }
        else{
            conditionCube = formatCondtion(scope,0,1,newValueBind,directiveValueBind,1);

            createType(useVar, header, writeSmvTypeTable, newValueBind, newValue, newValue->type);
            createAssign(useVar, header, writeSmvTypeTable, newValueBind, conditionCube, INIT, defaultValueBind);
        }
    }
    free(defaultValueBind);
    free(newValueBind);
    free(useVar);
}

void updateTime(HeaderSmv* main , STable * writeSmvTypeTable, char* newValue, int type, int typeExpr, int minmax)
{
    updateType("time", main, writeSmvTypeTable, newValue, type, minmax, NULL);
    typeExpr ? updateAssign("time",main,writeSmvTypeTable,newValue,NULL,type,NEXT,minmax) :
    updateAssign("time",main,writeSmvTypeTable,newValue,NULL,type,INIT,minmax);
}

char* createReferenceTDS(char* declaredName){
    return processActiveName(NULL,declaredName,-1,-1,TDS_ENTRY);
}

HeaderSmv* specHeader(smvtype type, char* name, int varP, int assignP, int transP, HeaderController* controller){
    HeaderSmv* newHeader = createHeader(type,name,varP,assignP,transP);
    addNewHeader(controller,newHeader);
    selectBuffer(VAR,"VAR\n",newHeader,0);
    selectBuffer(ASSIGN,"ASSIGN\n",newHeader,0);
    return newHeader;
}

void validateTdsDeclaration(char* declarationName, HeaderController* controller){
    if(lookup(controller->originalPorts,declarationName)){
        controller->validPorts++;
    }
}

int validateTdsTimeList(Object* encapsulatedTDS, TDS* newTDS, int C_TIME, int I_TIME, int F_TIME){
    int someIsValidToLazy = 0; // nenhum é valido = 0 , algum é valido = 1,  -1 a gente pode falar que algum é valido mas o primeiro é invalid (C_TIME = 0)
    int initialIsInvalid = 0;
    int i;
    for (i = 0; i < newTDS->DATA_TIME->OBJECT_SIZE; i++){
        Object* timeComponent = (Object*) newTDS->DATA_TIME->values[i];
        int time = *(int*) timeComponent->values[0];
        if(time < C_TIME){
            fprintf(stderr, "[WARNING] %s TDS's specification on time = %d was not evaluated. The specification was defined on a C_TIME  >= %d context!  \n",
                    encapsulatedTDS->SINTH_BIND,time,C_TIME);
            initialIsInvalid = time == 0? 1 : initialIsInvalid;
        }
        else if(time < I_TIME || time > F_TIME){
            fprintf(stderr, "[WARNING] %s TDS's specification on time = %d was not evaluated. The specification was defined out of the model time interval: %d ~ %d  \n",
                    encapsulatedTDS->SINTH_BIND,time,I_TIME,F_TIME);
        }
        else{
            if(!someIsValidToLazy){
                someIsValidToLazy = 1;
            }
            newTDS->COMPONENT_TIMES[time] = i;
        }
    }
    if(initialIsInvalid && someIsValidToLazy){
        someIsValidToLazy = -1; // senão vai ficar como 0 (então TODOS são invalidos)
    }
    return someIsValidToLazy;
}

void addTdsToLazyControl(Object* encapsulatedTDS, TDS* newTDS, HeaderController* controller, int C_TIME,  int I_TIME, int F_TIME){
    int addToLazy = newTDS->type != DATA_LIST? 1: validateTdsTimeList(encapsulatedTDS,newTDS,C_TIME, I_TIME,F_TIME);
    if(addToLazy){
        // só o inicial é invalido
        if(addToLazy == -1){
            createAssign("value",accessHeader(controller,PORTS,newTDS->SMV_REF),accessSmvInfo(controller,PORTS,newTDS->AUX_REF),"NULL",NULL,INIT,NULL);
        }
        controller->declaredPorts[controller->declaredPortsNumber] = newTDS;
        controller->declaredPortsNumber++;
    }else{
        createAssign("value",accessHeader(controller,PORTS,newTDS->SMV_REF),accessSmvInfo(controller,PORTS,newTDS->AUX_REF),"NULL",NULL,INIT,NULL);
        createAssign("value",accessHeader(controller,PORTS,newTDS->SMV_REF),accessSmvInfo(controller,PORTS,newTDS->AUX_REF),"value",NULL,NEXT,NULL);
    }
}

void addTdsOnPortsModule(char* moduleName, Object * newEncapsulatedTDS, TDS* newTDS, HeaderController* controller, int C_TIME,  int I_TIME, int F_TIME){
    char  declarationNameSmv[ALOC_SIZE_LINE];
    char  nameWithNoBreakL[ALOC_SIZE_LINE];
    int c = 0;
    while(*moduleName != '\n'){
        nameWithNoBreakL[c] = *moduleName;
        moduleName++;
        c++;
    }
    nameWithNoBreakL[c] = '\0';
    char* declarationName = newTDS->name? newTDS->name : newEncapsulatedTDS->SINTH_BIND;
    createType(declarationName,controller->PORTS_RELATED[0],controller->portsInfo[0],nameWithNoBreakL,NULL,-1);
    addTdsToLazyControl(newEncapsulatedTDS,newTDS,controller,C_TIME,I_TIME,F_TIME);
    validateTdsDeclaration(declarationName,controller);
}

void preProcessTDS(Object* encapsulatedTDS, HeaderController* controller, int C_TIME, int I_TIME, int F_TIME){
    TDS* SYNTH_TDS =  (TDS*)encapsulatedTDS->values[0];
    SYNTH_TDS->AUX_REF = controller->PORTS_INFO_CURRENT_SIZE;
    SYNTH_TDS->SMV_REF = controller->H_PORTS_CURRENT_SIZE;
    char moduleName[ALOC_SIZE_LINE];
    sprintf(moduleName,SmvConversions[ANY_BREAK_LINE],encapsulatedTDS->SINTH_BIND);
    HeaderSmv* newTdsHeader = specHeader(PORTS, moduleName, 0, 0, -1, controller);
    STable* auxTable = createTable(SMV_PORTS, NULL, 0, 0, -1);
    addNewAuxInfo(controller,auxTable);
    createType("value",newTdsHeader,auxTable,"NULL, 0, 1",NULL,TDS_ENTRY);
    addTdsOnPortsModule(newTdsHeader->moduleName, encapsulatedTDS, SYNTH_TDS, controller,C_TIME, I_TIME, F_TIME);
}

void specTDS(TDS* currentTDS, Object* lazyValue, int C_TIME, int I_TIME, HeaderController *controller, STable *currentScope) {

    //HeaderSmv* newTdsHeader = specHeader(PORTS, encapsulatedTDS->SINTH_BIND, 0, 0, -1, controller);
    //TDS* SYNTH_TDS =  (TDS*)encapsulatedTDS->values[0];
    HeaderSmv *currentHeader = accessHeader(controller, PORTS, currentTDS->SMV_REF);
    STable *currentInfo = accessSmvInfo(controller, PORTS, currentTDS->AUX_REF);
    if (currentTDS->type == DATA_LIST) {
        if (C_TIME == I_TIME) {
            createAssign("value", currentHeader, currentInfo, lazyValue->SINTH_BIND, NULL, INIT, NULL);
        } else {
            char *conditionCube = NULL;
            char *directiveValueBind = formatDirective(C_TIME);
            if (lookup(currentInfo, "next(value)")) {
                conditionCube = formatCondtion(currentScope, 0, 0, lazyValue->SINTH_BIND, directiveValueBind, 0);
                updateAssign("value", currentHeader, currentInfo, lazyValue->SINTH_BIND, conditionCube, TDS_ENTRY, NEXT,
                             -1);
            } else {
                conditionCube = formatCondtion(currentScope, 0, 0, lazyValue->SINTH_BIND, directiveValueBind, 1);
                createAssign("value", currentHeader, currentInfo, lazyValue->SINTH_BIND, conditionCube, NEXT, "NULL");
            }
        }
    }

}
//    char* valueString = "value";
//    char* defaultConditionEval = "NULL";
//    char* valueOnZero = "NULL"; // para caso a inicialização seja com NULL

    //Object* firstTimeComponent = (Object*) SYNTH_TDS->DATA_TIME->values[0];



    // testando synth (de ponteiro de funçoes) (tenho que salvar tambem o caminho do programa) (o escopo e controller mudam mas isso nao)

    //Object* valueComponent = (Object*) timeComponent->values[1];
    //(n,scope,controllerSmv);


 /*
    if(SYNTH_TDS->type == DATA_LIST){

        char result[ALOC_SIZE_LINE];
        char* parcialResult = malloc(sizeof(ALOC_SIZE_LINE)*sizeof(char));

        char* limitCondition = NULL;
        int limitedByTime = 0;

        char* components[SYNTH_TDS->DATA_TIME->OBJECT_SIZE]; // N strings. (no caso teste 3)

        int hasZeroTimeComponent = 0;
        int i;


        for (i = 0; i < SYNTH_TDS->DATA_TIME->OBJECT_SIZE; ++i) {

            Object* timeComponent = (Object*) SYNTH_TDS->DATA_TIME->values[1];
            int time = *(int*) timeComponent->values[0];
            Object* valueComponent = (Object*) timeComponent->values[1];
            char timeBind[ALOC_SIZE_LINE];
            sprintf(timeBind, "%d", time);


            // deve dar free
            char* valueBind = formatValueBind(NULL,NULL,valueComponent,0,0,0);

            if(C_TIME > time){
                fprintf(stderr, "[WARNING] DATA-LIST-DEFINITION FOR %s IS NOT VALID IN THE CURRENT TEMPORAL CONTEXT.\n CURRENT TIME = %d, USED TIME = %d,",
                        varName,C_TIME,time);
                limitedByTime = 1;
                // alocar limited condition (se já não foi alocada)
                if(!limitCondition) {
                    char CtimeBind[ALOC_SIZE_LINE];
                    sprintf(CtimeBind, "%d", C_TIME);
                    limitCondition = createConditionCube("next(time)", CtimeBind, "=", NULL, 0);
                }
            }
            if(!hasZeroTimeComponent && time == I_TIME) {
                hasZeroTimeComponent = 1;
            }
            // criar a string de múltiplas condições (lista)
            else{
                // deve criar a condição e avaliação
                char* componentConversion = createConditionCube("next(time)", timeBind, "=", NULL, 0);

                char* conditionCube = limitCondition ?
                createConditionCube(limitCondition, componentConversion, "&", valueBind, 1) :
                createConditionCube(componentConversion, "", "", valueBind, 1);
                free(componentConversion);
                if(i+1 == SYNTH_TDS->DATA_TIME->OBJECT_SIZE){
                    sprintf(parcialResult,)
                }
                else{

                }
            }

            else{
                if(i == 0){
                    sprintf(assignTo, SmvConversions[INIT], "value");
                    sprintf(componentBind,SmvConversions[ASSIGN_TO_TAB_BREAK_LINE],assignTo,"NULL");
                }
                if(time != 0){
                    char timeBind[ALOC_SIZE_LINE/2];
                    sprintf(timeBind,"%d",time);
                    sprintf(assignTo, SmvConversions[NEXT], valueString);

                }
            }



                // deve agora condiçao: next(time)
                char* specifiedCondition = createConditionCube("next(time)", timeBind, "=", NULL, 1);


        }
        // cria init(value) = NULL ou valor;
        createAssign(valueString,newTdsHeader,NULL,valueOnZero,limitCondition,INIT,"NULL");
        if(hasZeroTimeComponent && i == 1) {
            // sub-caso onde só temos a inicialização (e o next deve atribuir como NULL para todos os seguintes)
            createAssign(valueString,newTdsHeader,NULL,"NULL",NULL,NEXT,"NULL");
        }
        else{
            // caso onde temos (OU NÃO) a zeroTime)
            // deve criar um next para os demais (fazendo uso de todas as componentes)
            selectBuffer(ASSIGN,result,newTdsHeader,0);
        }
    }
    */



/*
void letGoOldEntry(TableEntry* var, STable* refAuxTable){
    char* useVar = NULL;
    // temos que usar escopo de VAR não o escopo atual de onde a chamada ocorre!
    // como nesse caso é necessário referênciar EXATAMENTE o nome da variável,

    int redefNum = var->val->redef == 0? 0 : var->val->redef-1;
    useVar = processActiveName(var->parentScope, var->name, 1, 0, 0);
    char varInit[ALOC_SIZE_LINE/2];
    char varNext[ALOC_SIZE_LINE/2];
    sprintf(varInit,SmvConversions[INIT],useVar);
    sprintf(varNext,SmvConversions[NEXT],useVar);

    letGoEntryByName(refAuxTable,useVar);
    letGoEntryByName(refAuxTable,varInit);
    letGoEntryByName(refAuxTable,varNext);
    free(useVar);
}
*/

void writeResultantHeaders(HeaderController* controller, const char* path){
  
  int i;
  FILE* smvoutput = fopen(path, "w");


  writeHeader(controller->MAIN,smvoutput);
  for(i = 0; i< controller->H_FUNCTION_CURRENT_SIZE; i++){
        writeHeader(controller->FUNCTIONS[i],smvoutput);
  }
  for(i = 0; i< controller->H_AUTOMATA_CURRENT_SIZE; i++){
        writeHeader(controller->AUTOMATA_RELATED[i],smvoutput);
  }
  for(i = 0; i< controller->H_PORTS_CURRENT_SIZE; i++){
        writeHeader(controller->PORTS_RELATED[i],smvoutput);
  }
  fclose(smvoutput);

}


