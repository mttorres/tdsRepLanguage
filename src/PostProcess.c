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



char *createConditionCube(char *opBind1, char *opBind2, char *operation, char *evaluation, int firstCond, int concCube)
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

char* formatBinds(int ctime, int changeContext, char* directiveValueBind, char* valueBind, char* defaultValueBind,
                  Object* expr, STable* scope, int firstCondition){

    sprintf(directiveValueBind, "%d", ctime); // SINTH_BIND da diretiva temporal corrente
    copyValueBind(expr,valueBind,0,0); // SINTH_BIND da expressão, pode variar para vetores e estruturas complexas

    char* condition = scope->type == IF_BLOCK || ELSE_BLOCK? scope->conditionBind : NULL; // SINTH_BIND do escopo
    // SINTH_BIND da condição temporal, ex: "next(time) = 2"
    char* temporalCondition = changeContext?
                              createConditionCube("next(time)",directiveValueBind, "=", NULL,1,0)
                                           : NULL;

    // ponteiro auxiliar que referencia a condição temporal criada
    char* auxReftemporalCondition = temporalCondition;

    // se existe alguma condição vinda do escopo e uma condição temporal, as concatena, senão usa apenas uma delas ou nenhuma delas
    char* conditionCube = temporalCondition && condition?
                          createConditionCube(auxReftemporalCondition,condition,"&",valueBind,firstCondition,1) :
                          auxReftemporalCondition? createConditionCube(auxReftemporalCondition,"", "", valueBind,firstCondition,0) :
                          condition? createConditionCube(condition,"", "", valueBind,firstCondition,0) : NULL;

    // libera o cubo sem avaliação utilizado anteriormente para o passo anterior
    if(temporalCondition){
        free(temporalCondition);
    }

    // "otimização" para criar o caso default, se necessário
    if(conditionCube){
        copyValueBind(expr,defaultValueBind,0,1);
    }

    return conditionCube;
}


void createType(char* varName ,HeaderSmv* header, STable* writeSmvTypeTable, const char* newValue, int type)
{
    char* newType = malloc(sizeof(char)*ALOC_SIZE_LINE);
    if(type == NUMBER_ENTRY || type == T_DIRECTIVE_ENTRY){
        sprintf(newType,SmvConversions[INTERVAL_DEC],varName,newValue,newValue);
        char* auxDelim = strstr(newType,":");
        char* auxFim = strstr(auxDelim,"..");
        int pointIni = (auxDelim-newType+2);
        int pointEnd = ((auxFim-newType))-1;
        int pos = header->VAR_POINTER;
        int tam = strlen(newType);
        void* po[] = {&pos, &tam, &pointIni, &pointEnd};
        addValue(varName, po, WRITE_SMV_INFO, 4, 0, writeSmvTypeTable, 0);
    }
    if(type == LOGICAL_ENTRY){
        sprintf(newType,SmvConversions[BOOLEAN_DEC],varName,newValue,newValue);
        int pos = header->VAR_POINTER;
        int tam = strlen(newType);
        void* po[] = {&pos, &tam};
        addValue(varName, po, WRITE_SMV_INFO, 2, 0, writeSmvTypeTable, 0);

    }
    if(type == TDS_ENTRY || LABEL_ENTRY)
    {
        //void* po[] = {&pos, &tam};
        //addTypeSet(varName,po,TYPE_SET,2,writeSmvTypeTable);
    }
    header->varBuffer[header->VAR_POINTER] = newType;
    header->VAR_POINTER += 1;
}

void updateType(char* varName ,HeaderSmv* header, STable* writeSmvTypeTable, const char* newValue, int type,int minmax)
{
    // começando com numérico x..y;
    // criar enum mapeador ao decorrer...
    if(type == NUMBER_ENTRY || type == T_DIRECTIVE_ENTRY)
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
        sprintf(exprResultString,SmvConversions[ASSIGN_TO_TAB_BREAK_LINE],exprInterL,exprInterR);

        char* auxChPoint;
        auxChPoint = strstr(exprResultString,";\n"); // devolve exatamente o ponto, devemos ir para o \n
        long dif = auxChPoint-exprResultString;
        size = strlen(exprResultString);
        pointIni = dif+1;
        pointEnd = pointIni;

        free(condition);
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

char* processActiveName(char* varName, char* funcRef, int redef, int level, int order, char* interScope, char* interRedef){

    char* useVar = NULL;
    // qualquer escopo diferente de GLOBAL/MAIN
    if(order || level){
        //(função)
        if(funcRef){
            if(!level){
                sprintf(interScope,SmvConversions[NAME_BY_FUNC_SCOPE],varName,funcRef);
            }
            else{
                sprintf(interScope,SmvConversions[NAME_BY_FUNC_SSCOPE],varName,funcRef,level,order);
            }
        }
        // if, else, fors ....
        else{
            sprintf(interScope,SmvConversions[NAME_BY_SCOPE],varName,level,order);
        }
        useVar = interScope;
    }
    else{
        useVar = varName;
    }
    if(redef){
        sprintf(interRedef,SmvConversions[REDEF_NAME],useVar,redef);
        useVar = interRedef;
    }
    return useVar;
}

void specAssign(char *varName, HeaderSmv *header, STable *writeSmvTypeTable, char *newValue, char *condition,
                char *defaultValue, int redef, char *funcRef, int order, int level, int type, int typeExpr, int minmax)
{
    char* useVar = NULL; // por default, usamos o nome da varável (se não for, em escopos diferentes ou ainda em redef )
    char interScope[ALOC_SIZE_LINE];  //nome com info de scope
    char interRedef[ALOC_SIZE_LINE];  //nome com redefinição
    char statevarname[ALOC_SIZE_LINE]; // init ou next
    useVar = processActiveName(varName,funcRef,redef,level,order,interScope,interRedef);

    // next
    if(typeExpr){
        // criar init/next(useVar)
        sprintf(statevarname,SmvConversions[NEXT],useVar);
        //verifica se existe next(statevarname)
        updateType(useVar,header,writeSmvTypeTable,newValue,type,minmax);
        if(lookup(writeSmvTypeTable,statevarname)){
            updateAssign(useVar,header,writeSmvTypeTable,newValue,condition,type,NEXT,minmax);
        }
        else{
            createAssign(useVar, header, writeSmvTypeTable, newValue, condition, NEXT, NULL);
        }
    }
    // init
    else{
        createType(useVar,header,writeSmvTypeTable,newValue,type);
        createAssign(useVar, header, writeSmvTypeTable, newValue, condition, INIT, defaultValue);
    }
}

void updateTime(HeaderSmv* main , STable * writeSmvTypeTable, char* newValue, int type, int typeExpr, int minmax)
{
    updateType("time",main,writeSmvTypeTable,newValue,type,minmax);
    typeExpr ? updateAssign("time",main,writeSmvTypeTable,newValue,NULL,type,NEXT,minmax) :
    updateAssign("time",main,writeSmvTypeTable,newValue,NULL,type,INIT,minmax);
}


void writeResultantHeaders(HeaderController* controller, const char* path){
  
  int i;
  FILE* smvoutput = fopen(path, "w");
  for(i = 0; i< controller->CURRENT_SIZE; i++){
		writeHeader(controller->headers[i],smvoutput);
  }
  fclose(smvoutput);

}


