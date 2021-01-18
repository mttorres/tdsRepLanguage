#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../headers/PostProcess.h"


typedef enum MAP_CONVERSIONS { ANY, ANY_TERM, ANY_BREAK_LINE, UN_OP, OP, REDEF_NAME, NAME_BY_SCOPE, NAME_BY_FUNC_SCOPE, NAME_BY_FUNC_SSCOPE,
                               INIT, NEXT, ASSIGN_TO,
                               ASSIGN_TO_TAB_BREAK_LINE, CASE, CASE_EVAL, N_CASE_EVAL, DEFAULT_CASE_EVAL, EQUAL_CASE_EVAL,
                               INTERVAL_DEC, BOOLEAN_DEC ,SET, PAR } MAP_CONVERSIONS;

                                                // ex: 1 + 1
char* SmvConversions[] = {"%s", "%s;",  "%s \n", "%s%s", "%s %s %s ", "%s_redef%d%", "%s_scope%d_%d", "%s_scope%s","%s_scope%s_%d_%d",
                          "init(%s)", "next(%s)", "%s:= %s;",
                          "\t%s:= %s;\n",  "case \n\t\t%s\n\t\tTRUE : %s; \n\tesac",
                          "%s : %s;", "\n\t\t%s : %s;\n", "TRUE : %s; \n", "%s = %s : %s; \n",
                          "\t%s : %s..%s;\n", "\t%s : boolean;\n" , "\t%s : {%s};", "%s, %s" };

int  ALOC_SIZE_LINE = 300;


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
        char* cube = malloc(sizeof(strlen(opBind1)+strlen(opBind2)+strlen(operation)+1));

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
    scope->conditionBind = malloc(strlen(conditionExpr->SINTH_BIND)+1);
    strcpy(scope->conditionBind,conditionExpr->SINTH_BIND);
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


void createType(char *varName, HeaderSmv *header, STable *writeSmvTypeTable, const char *newValueBind, Object *newValue,
                int type)
{
    char* newType = malloc(sizeof(char)*ALOC_SIZE_LINE);
    if(type == NUMBER_ENTRY || type == T_DIRECTIVE_ENTRY){
        sprintf(newType, SmvConversions[INTERVAL_DEC], varName, newValueBind, newValueBind);
        char* auxDelim = strstr(newType,":");
        char* auxFim = strstr(auxDelim,"..");
        int pointIni = (auxDelim-newType+2);
        int pointEnd = ((auxFim-newType))-1;
        int pos = header->VAR_POINTER;
        int tam = strlen(newType);
        int minmax = newValue? *(int*) newValue->values[0] : 0;
        void* po[] = {&pos, &tam, &pointIni, &pointEnd, &minmax, &minmax};
        addValue(varName, po, WRITE_SMV_INFO, 6, 0, writeSmvTypeTable, 0);
    }
    if(type == LOGICAL_ENTRY){
        sprintf(newType, SmvConversions[BOOLEAN_DEC], varName, newValueBind, newValueBind);
        int pos = header->VAR_POINTER;
        int tam = strlen(newType);
        void* po[] = {&pos, &tam};
        addValue(varName, po, WRITE_SMV_INFO, 2, 0, writeSmvTypeTable, 0);

    }
    if(type == TDS_ENTRY || LABEL_ENTRY)
    {
        //void* po[] = {&pos, &tam};
        //addTypeSetSmv(varName,po,TYPE_SET,2,writeSmvTypeTable);
    }
    header->varBuffer[header->VAR_POINTER] = newType;
    header->VAR_POINTER += 1;
}

void updateType(char* varName ,HeaderSmv* header, STable* writeSmvTypeTable, const char* newValue, int type,int minmax)
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
            updateSubStringInterval(newValue,header->varBuffer[pos],sizeNew,pointIni,pointEnd,size,&newPointIni,&newPointEnd);
            size = -1*((pointEnd-pointIni+1) - sizeNew) + size;
            void* vpSize[] = {&size};
            updateValue(varName, vpSize, WRITE_SMV_INFO, 1, 1, -1, writeSmvTypeTable, 0);

            // atualizar o fim do intervalo não mudar a nossa variável pointEnd também! Só atualiza o tamanho
            if(!minmax)
            {
                void* vpInEnd[] = {&newPointEnd};
                updateValue(varName, vpInEnd, WRITE_SMV_INFO, 1, 3, -1, writeSmvTypeTable, 0);
                int min = *(int*) newValue;
                void* vpmin[] = {&min};
                updateValue(varName, vpmin, WRITE_SMV_INFO, 1, 4, -1, writeSmvTypeTable, 0);
            }
            else{
                int max = *(int*) newValue;
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
            sprintf(exprInterR,SmvConversions[CASE],condition,varName); // default sendo o inicial
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
    //atualiza tabela auxiliar para init/next(var)
    void* po[] = {&pos, &size,&pointIni,&pointEnd};
    addValue(exprInterL, po, WRITE_SMV_INFO, 4, 0, writeSmvTypeTable, 0);
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
    updateSubStringInterval(newValue, updated, sizeNew, pointIni, pointEnd, size, &newPointInit, &newPointEnd);

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

char *processActiveName(char *varName, int redef, int level, int order, int notExistsOutScope) {

    char* useVar = NULL;
    char interScope[ALOC_SIZE_LINE];  //nome com info de scope
    char interRedef[ALOC_SIZE_LINE];  //nome com redefinição
    // qualquer escopo diferente de GLOBAL/MAIN
    if(notExistsOutScope && (order || level) ){
        // if, else, fors ....
        sprintf(interScope,SmvConversions[NAME_BY_SCOPE],varName,level,order);
        useVar = interScope;
    }
    else{
        useVar = varName;
    }
    if(redef){
        sprintf(interRedef,SmvConversions[REDEF_NAME],useVar,redef);
        useVar = interRedef;
    }
    char* activeName = malloc(sizeof(ALOC_SIZE_LINE));
    strcpy(activeName,useVar);
    return activeName;
}

Object* refCopyOfVariable(TableEntry* var){
    char* useVar = NULL;
    // temos que usar escopo de VAR não o escopo atual de onde a chamada ocorre!
    // como nesse caso é necessário referênciar EXATAMENTE o nome da variável,
    useVar = processActiveName(var->name, var->val->redef, var->parentScope->level, var->parentScope->order, 1);
    Object* copyRef = copyObject(var->val);
    if(useVar){
        free(copyRef->SINTH_BIND);
//      copyRef->SINTH_BIND[0] = '\0';
        copyRef->SINTH_BIND =  malloc(sizeof(char)*strlen(useVar) + 1);
        strcpy(copyRef->SINTH_BIND,useVar);
    }
    free(useVar);
    return copyRef;
}

void specAssign(int varInit, int contextChange, TableEntry *var, HeaderSmv *header, STable *scope, STable *writeSmvTypeTable,
           Object *newValue, int redef, int typeExpr, int C_TIME)
{
    // strings para binds
    //binds da expressão
    char newValueBind[ALOC_SIZE_LINE]; //vai ser usado para o valor passado para o assign
    char defaultValueBind[ALOC_SIZE_LINE]; //valor default para expressões next que necessitem inicialização (ex: init = 0, para inteiros)
    char* conditionCube = NULL ; // cubo de condições (mudança de contexto ou escopo if/else)

    // decide o nome apropriado para a variável
    char* useVar = NULL; // por default, usamos o nome da varável (se não for, em escopos diferentes ou ainda em redef )
    char statevarname[ALOC_SIZE_LINE]; // init ou next
    useVar = processActiveName(var->name, redef, var->level, var->order, varInit);

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
    // next
    if(typeExpr){
        // criar init/next(useVar)
        sprintf(statevarname,SmvConversions[NEXT],useVar);
        //verifica se existe next(statevarname)
        if(lookup(writeSmvTypeTable,statevarname)){
            conditionCube = formatBinds(C_TIME, contextChange, defaultValueBind, newValueBind, defaultValueBind,
                                        newValue, scope, 0, 0, 0, 0);
            updateAssign(useVar, header, writeSmvTypeTable, newValueBind, conditionCube, newValue->type, NEXT, minmax);
        }
        else{
            conditionCube = formatBinds(C_TIME, contextChange, defaultValueBind, newValueBind, defaultValueBind,
                                        newValue, scope, 1, 0, 0, 0);
            createAssign(useVar, header, writeSmvTypeTable, newValueBind, conditionCube, NEXT, NULL);
        }
        updateType(useVar, header, writeSmvTypeTable, newValueBind, newValue->type, minmax);
    }
    // init casos
    // caso 1 :  init default + next ( seja de variável que só existe em t>0(pode também ser uma redefinição) (devem ignorar condições nesse init)
    // caso 2 : init unico, seja para variáveis que foram redefinidas em t = 0 ou declarações comuns
    else{
        if((C_TIME && redef) || contextChange){
            conditionCube = formatBinds(C_TIME, contextChange, defaultValueBind, newValueBind, defaultValueBind,
                                        newValue,
                                        scope, 1, varInit, 1, 1);

            createType(useVar, header, writeSmvTypeTable, defaultValueBind, NULL, newValue->type);
            createAssign(useVar, header, writeSmvTypeTable, defaultValueBind, conditionCube, INIT, defaultValueBind);
        }
        else{
            conditionCube = formatBinds(C_TIME, contextChange, defaultValueBind, newValueBind, defaultValueBind,
                                        newValue,
                                        scope, 1, varInit, 1, 0);
            createType(useVar, header, writeSmvTypeTable, newValueBind, newValue, newValue->type);
            createAssign(useVar, header, writeSmvTypeTable, newValueBind, conditionCube, INIT, defaultValueBind);
        }
    }
    free(useVar);
}

void updateTime(HeaderSmv* main , STable * writeSmvTypeTable, char* newValue, int type, int typeExpr, int minmax)
{
    updateType("time",main,writeSmvTypeTable,newValue,type,minmax);
    typeExpr ? updateAssign("time",main,writeSmvTypeTable,newValue,NULL,type,NEXT,minmax) :
    updateAssign("time",main,writeSmvTypeTable,newValue,NULL,type,INIT,minmax);
}

void letGoOldEntry(TableEntry* var, STable* refAuxTable){
    char* useVar = NULL;
    // temos que usar escopo de VAR não o escopo atual de onde a chamada ocorre!
    // como nesse caso é necessário referênciar EXATAMENTE o nome da variável,

    int redefNum = var->val->redef == 0? 0 : var->val->redef-1;
    useVar = processActiveName(var->name, redefNum, var->parentScope->level, var->parentScope->order, 1);
    char varInit[ALOC_SIZE_LINE/2];
    char varNext[ALOC_SIZE_LINE/2];
    sprintf(varInit,SmvConversions[INIT],useVar);
    sprintf(varNext,SmvConversions[NEXT],useVar);

    letGoEntryByName(refAuxTable,useVar);
    letGoEntryByName(refAuxTable,varInit);
    letGoEntryByName(refAuxTable,varNext);
    free(useVar);
}


void writeResultantHeaders(HeaderController* controller, const char* path){
  
  int i;
  FILE* smvoutput = fopen(path, "w");
  for(i = 0; i< controller->CURRENT_SIZE; i++){
		writeHeader(controller->headers[i],smvoutput);
  }
  fclose(smvoutput);

}


