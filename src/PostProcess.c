#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../headers/PostProcess.h"


typedef enum MAP_CONVERSIONS { ANY, ANY_TERM, ANY_BREAK_LINE, UN_OP, OP, REDEF_NAME, NAME_BY_SCOPE, NAME_SSCOPE,
                               INIT, NEXT, ASSIGN_TO,
                               ASSIGN_TO_TAB_BREAK_LINE, CASE, CASE_EVAL, N_CASE_EVAL, DEFAULT_CASE_EVAL, EQUAL_CASE_EVAL,
                               INTERVAL_DEC, BOOLEAN_DEC ,SET, V_MODULE_DEC ,PAR, TDS_MODULE_NAME, TDS_VALUE_REF,
                               TDS_DELAYED_EXPR_NEXT, TDS_DELAYED_EXPR_NEXT_TIMEV,
                               TDS_INPUT_EXPR, TDS_INPUT_EXPR_TIMEV,
                               MODULE_BREAK_LINE  } MAP_CONVERSIONS;

                                                // ex: 1 + 1
char* SmvConversions[] = {"%s", "%s;",  "%s\n", "%s%s", "%s %s %s", "%s_redef%d%", "%s_scope%d_%d","%s_scope%d_%d_%d_%d",
                          "init(%s)", "next(%s)", "%s:= %s;",
                          "\t%s:= %s;\n",  "case \n\t\t%s\n\t\tTRUE : %s; \n\tesac",
                          "%s : %s;", "\n\t\t%s : %s;\n", "TRUE : %s; \n", "%s = %s : %s; \n",
                          "\t%s : %d..%d;\n", "\t%s : boolean;\n" , "\t%s : {%s};\n", "\t%s : %s;\n", "%s, %s", "tds_%s", "%s.value",
                          "%s = NULL : %s.value;\n\t\t%s.value = NULL & %s != NULL : NULL;",  "next(time) > %d & %s = NULL : %s.value;\n\t\tnext(time) > %d & %s.value = NULL & %s != NULL : NULL;",
                          "%s = NULL : %s;\n\t\t%s = NULL : %s;",  "%s > %d & %s = NULL : %s;\n\t\t%s > %d & %s = NULL : %s;",
                          "MODULE %s\n" };

int  ALOC_SIZE_LINE = 300;
int  MULTIPLIER_SIMPLE_HASH = 1;

/**
 *
 * @param o
 * @param bind
 * @param index
 * @param defaultValue
 * @param valueNotRef serve para quando atualizarmos um type-set (só considera os valores possíveis).
 * Assim, a conversão não vai pegar o nome da variável ou a expressão e sim o resultado
 */
void copyValueBind(Object *o, char *bind, int index, int defaultValue, int valueNotRef)
{
    char* formatS = "%s";
    char* formatN = "%d";
    char* formatRef = "%s[%d]";

    if(!index) {
        if (!defaultValue && o->SINTH_BIND && !valueNotRef) {
            sprintf(bind, formatS, o->SINTH_BIND);
        } else {
            if (o->type == NUMBER_ENTRY || o->type == T_DIRECTIVE_ENTRY) {
                    sprintf(bind, formatN, defaultValue? 0 : *(int *) o->values[0]);
            }
            if (o->type == LOGICAL_ENTRY) {
                sprintf(bind, formatS, *(int *) o->values[0] && !defaultValue ? "TRUE" : "FALSE");
            }
            if (o->type == LABEL_ENTRY) {
                sprintf(bind, formatS, defaultValue? "NULL" : (char *) o->values[0]);
            }
            if(o->type == NULL_ENTRY){
                sprintf(bind, formatS, "NULL");
            }
            if (o->type == TDS_ENTRY) {
                // ...
            }
        }
    }
    else{
        // listas vão ser objetos (de tamanho > 1) que guardam objetos
        Object* listComponent = (Object*) o->values[index];
        copyValueBind(o, bind, 0, defaultValue, 0);
    }
}

void createExprBind(char *result, Object *o1, Object *o2, char *op) {
    if(!o2){
        sprintf(result,SmvConversions[UN_OP],op,o1->SINTH_BIND);
    }
    else{
        sprintf(result,SmvConversions[OP],o1->SINTH_BIND,op,o2->SINTH_BIND);
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
    copyValueBind(expr, valueBind, 0, 0, 0); // SINTH_BIND da expressão, pode variar para vetores e estruturas complexas

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
        copyValueBind(expr, defaultValueBind, 0, 1, 0);
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
        sprintf(newType, SmvConversions[BOOLEAN_DEC], varName);
        int tam = strlen(newType);
        void* po[] = {&pos, &tam};
        addValue(varName, po, WRITE_SMV_INFO, 2, 0, writeSmvTypeTable, 0);

    }
    else if(type == TDS_ENTRY || type == LABEL_ENTRY || type == NULL_ENTRY)
    {
        sprintf(newType, SmvConversions[SET], varName, newValueBind);
        int tam = strlen(newType);
        addTypeSetSmv(varName,pos,tam,newValueBind,type,writeSmvTypeTable);
    }
    // cria uma variável que é instancia de modulo nuXmv
    else{
        sprintf(newType,SmvConversions[V_MODULE_DEC],varName,newValueBind);
        int tam = strlen(newType);
        void* po[] = {&pos, &tam};
        addValue(varName, po, WRITE_SMV_INFO, 2, 0, writeSmvTypeTable, 0);
    }
    header->varBuffer[header->VAR_POINTER] = newType;
    header->VAR_POINTER += 1;
}

/**
 * Realiza as operações de hash e atualização de um type-set
 * @param newValue o novo valor a ser adicionado ao conjunto
 * @param varName o nome da variável
 * @param writeSmvTypeTable a tabela de simbolos auxiliar
 * @param header o header
 * @SideEffects: Altera as informações do objeto type-set recuperado da tabela auxiliar
 */
void updateTypeSet(char* newValue, char* varName, STable* writeSmvTypeTable, HeaderSmv* header)
{
    TableEntry* entryTypeSetInfo;
    entryTypeSetInfo =  lookup(writeSmvTypeTable,varName);
    int pos = *(int*) entryTypeSetInfo->val->values[0];
    int size = *(int*) entryTypeSetInfo->val->values[1];
    int* hash_set = (int*) entryTypeSetInfo->val->values[2];
    //int usedSize = *(int*) entryTypeSetInfo->val->values[3];

    int hashForNewValue = hash(newValue, MAX_SIMPLE);
    if(!hash_set[hashForNewValue]) {
        hash_set[hashForNewValue] = 1; // por ser uma ED nao precisa "reatualizar"
        char* original = header->varBuffer[pos];
        char* originalRef = original; // impede sideEffects (exemplo ele incrementar dentro de addparams e dar free errado mais a diante)
        char* newTypeSet = addParams(original,newValue,"{","}");
        header->varBuffer[pos] = newTypeSet;
        free(original);
    }
    else{
        // warning
    }
}

void updateType(char *varName, HeaderSmv *header, STable *writeSmvTypeTable, const char *newValueBind, int type, int minmax,
                Object *newValue)
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
            int sizeNew = strlen(newValueBind);
            // min..max;
            if(minmax)
            {
                pointIni = pointEnd+3; // n..max;
                // nota! o size já está indexbased!
                pointEnd = header->varBuffer[pos][size-1] == '\n' ? size-3  : size-2; // max;\n (-1 do index based) - (-2 ou -1 dependendo do fim)
                //size = header->varBuffer[pos][size-1] == '\n' ? size-1 : size;
            }
            updateSubStringInterval(newValueBind, header->varBuffer[pos], sizeNew, pointIni, pointEnd, size, &newPointIni,
                                    &newPointEnd, 0);
            size = -1*((pointEnd-pointIni+1) - sizeNew) + size;
            void* vpSize[] = {&size};
            updateValue(varName, vpSize, WRITE_SMV_INFO, 1, 1, -1, writeSmvTypeTable, 0);

            // atualizar o fim do intervalo não mudar a nossa variável pointEnd também! Só atualiza o tamanho
            if(!minmax)
            {
                void* vpInEnd[] = {&newPointEnd};
                updateValue(varName, vpInEnd, WRITE_SMV_INFO, 1, 3, -1, writeSmvTypeTable, 0);
                int min = newValue ? *(int*) newValue->values[0] : 0;
                void* vpmin[] = {&min};
                updateValue(varName, vpmin, WRITE_SMV_INFO, 1, 4, -1, writeSmvTypeTable, 0);
            }
            else{
                int max = newValue ? *(int*) newValue->values[0] : 1;
                void* vpmax[] = {&max};
                updateValue(varName, vpmax, WRITE_SMV_INFO, 1, 5, -1, writeSmvTypeTable, 0);
            }
        }
//        else{
//            printf("[updateType] WARNING: type of %s not declared on headers \n",varName);
//        }
    }
    if(type == TDS_ENTRY || type == LABEL_ENTRY || type == NULL_ENTRY){
        char rawValueBind[ALOC_SIZE_LINE/2];
        copyValueBind(newValue,rawValueBind,0,0,1);
        updateTypeSet(rawValueBind,varName,writeSmvTypeTable,header);
    }
}
/**
 * Encapsula as operações básicas usadas para criar um assign tipo init/next
 * @param statenamevar o nome de estado da variavel
 * @param varName o nome da variável
 * @param newValue o bind do novo valor
 * @param condition a condição de um case junto de sua avaliação
 * @param typeExpr o tipo de expressão (init/next)
 * @param defaultEvalCond a avaliação default do case que será construído (ex: TRUE : nomeVariável ou TRUE: NULL)
 * @param freeCondition uma flag usada para verificar a necessidade de dar free em uma condição passada como parâmetro (usado para simplificar responsabilidades)
 * @param size o ponteiro para o tamanho da nova string, calculado e passado para função chamadora
 * @param pointIni ponteiro para o ponto de inicio do intervalo de interesse da nova string, calculado e passado para função chamadora
 * @param pointEnd ponteiro para o ponto de fim do intervalo de interesse da nova string, calculado e passado para função chamadora
 * @return um assign em string ao computar todas as informações.
 * @SideEffects:  Para generalidade, os variáveis fornecidas pelo chamador (size, pointIni, pointEnd) são atualizadas a medida
 * que a nova string é constrúida. Além disso o retorno dessa função deve ser liberado da memória após uso.
 */
char * assignCreationOperations(char* statenamevar, char *varName, const char *newValue, char *condition, int typeExpr, char *defaultEvalCond,
                                int freeCondition, int *size, int *pointIni, int *pointEnd) {

    char* exprResultString = malloc(sizeof(char)*ALOC_SIZE_LINE);
    char exprInterR[ALOC_SIZE_LINE];
    // parâmetros a ser salvo na tabela auxiliar SMV

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
        if(freeCondition){
            free(condition);
        }
        sprintf(exprResultString,SmvConversions[ASSIGN_TO_TAB_BREAK_LINE],statenamevar,exprInterR);

        char* auxChPoint;
        auxChPoint = strstr(exprResultString,";\n"); // devolve exatamente o ponto, devemos ir para o \n
        long dif = auxChPoint-exprResultString;
        *size = strlen(exprResultString);
        *pointIni = dif+1;
        *pointEnd = *pointIni;
    }
    else{
        // atualiza o init/next dessa função
        // criar a expressão definitiva com sprintf?
        // pros : é facil
        // cons: não tenho controle sobre qual parte da string minha expressão começa ou termina...
        // NA VERDADE EU TENHO, posIni:  até o = (posso salvar em um array mapeador também)
        //  posEnd = posIni + tamanhoString(expressão)

        sprintf(exprInterR,SmvConversions[ANY],newValue);
        sprintf(exprResultString,SmvConversions[ASSIGN_TO_TAB_BREAK_LINE],statenamevar,exprInterR);

        *size = strlen(exprResultString);
        *pointIni = (strlen(statenamevar) - 1) + 5; //  %s:= %s (+2 para pular ele mesmo e o : e depois = e espaço, e fora o \t no inicio)
        *pointEnd = *pointIni + strlen(exprInterR)-1;

    }
    return exprResultString;
}


// quebrar em spec next e spec init
void createAssign(char *varName, HeaderSmv *header, STable *writeSmvTypeTable, const char *newValue, char *condition,
                  int typeExpr, char *defaultEvalCond, int freeCondition, int initialNext)
{
    int pos;
    int size;
    int pointIni;
    int pointEnd;
    char exprInterL[ALOC_SIZE_LINE];
    sprintf(exprInterL, SmvConversions[typeExpr], varName);
    char* assign = assignCreationOperations(exprInterL,varName, newValue, condition, typeExpr,
                                            defaultEvalCond, freeCondition, &size, &pointIni, &pointEnd);

    // escreve atribuição no buffer
    // encapsular em método(!)
    header->assignBuffer[header->ASSIGN_POINTER] = assign;
    pos = header->ASSIGN_POINTER;
    header->ASSIGN_POINTER++;

    //atualiza tabela auxiliar para init/next(var) (se necessário)
    if(writeSmvTypeTable) {
        void* po[] = {&pos, &size,&pointIni,&pointEnd,&initialNext};
        addValue(exprInterL, po, WRITE_SMV_INFO, 5, 0, writeSmvTypeTable, 0);
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
        copyValueBind(expr, valueBind, index, isDefault, 0);
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

/**
 * Dado que é possível criar ASSIGNS NEXT default, vale verificar se devemos sobrescrever esse após a variável
 * não ser mais não deterministica.
 * @param writeSmvTypeTable a tabela de simbolos auxiliar
 * @param statevarname o nome de estado da variável
 * @param headerSmv o header auxiliar
 * @return 1 caso exista um assign next default
 */
int checkFristNext(STable* writeSmvTypeTable, char* statevarname, HeaderSmv* headerSmv){
    TableEntry * defaultNextReg = lookup(writeSmvTypeTable,statevarname);
    int isFirst = *(int*) defaultNextReg->val->values[4];
    if(defaultNextReg && isFirst){
        return 1;
    }
    else{
        return 0;
    }
}

/**
 * Cria uma string de assign NEXT sobrescrevendo todas as informações dessa na tabela de simbolos auxiliar.
 * @param statevarname o nome de estado da variável
 * @param varName o nome da variável
 * @param header o header utilizado
 * @param writeSmvTypeTable a tabela auxiliar
 * @param newValue o novo valor em string
 * @param condition a condição para avaliação do valor
 * @param defaultEvalCond a condição default de avaliação
 */
void overwriteAssign(char* statevarname, char *varName, HeaderSmv *header, STable *writeSmvTypeTable, const char *newValue,
                     char *condition, char *defaultEvalCond){

    int pos;
    int size;
    int pointIni;
    int pointEnd;
    int initialNext = 0;
    char* assign = assignCreationOperations(statevarname,varName, newValue, condition, NEXT,
                                            defaultEvalCond, 1, &size, &pointIni, &pointEnd);

    TableEntry * defaultNextReg = lookup(writeSmvTypeTable,statevarname);
    pos = *(int*) defaultNextReg->val->values[0];
    int sizeOld = *(int*) defaultNextReg->val->values[1];
    memset(header->assignBuffer[pos],0,sizeOld);

    int hashforreg = calculateHashPos(statevarname,writeSmvTypeTable);
    writeSmvTypeTable->tableData[hashforreg] = NULL;
    letgoEntry(defaultNextReg);

    strcpy(header->assignBuffer[pos],assign);
    free(assign);

    //sobrescreve tabela auxiliar para next
    void* po[] = {&pos, &size,&pointIni,&pointEnd,&initialNext};
    addValue(statevarname, po, WRITE_SMV_INFO, 5, 0, writeSmvTypeTable, 0);
}

/**
 * Cria um assign NEXT default, para evitar variáveis com estados futuros não deterministicos
 * @param useVar o nome da variável
 * @param header o header atual
 * @param writeSmvTypeTable a tabela de simbolos auxiliar
 */
void createDefaultNext(char* useVar, HeaderSmv* header, STable* writeSmvTypeTable){
    createAssign(useVar, header, writeSmvTypeTable, useVar, NULL, NEXT, NULL, 1, 1);
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
            if(checkFristNext(writeSmvTypeTable,statevarname,header)){
                conditionCube = formatCondtion(scope,0,0,newValueBind,directiveValueBind,1);
                overwriteAssign(statevarname,varName,header,writeSmvTypeTable,newValueBind,conditionCube,defaultValueBind);
            }
            else{
                conditionCube = formatCondtion(scope,0,0,newValueBind,directiveValueBind,0);
                updateAssign(useVar, header, writeSmvTypeTable, newValueBind, conditionCube, newValue->type, NEXT, minmax);
            }
        }
        else{
            conditionCube = formatCondtion(scope,0,0,newValueBind,directiveValueBind,1);
            createAssign(useVar, header, writeSmvTypeTable, newValueBind, conditionCube, NEXT, NULL, 1, 0);
        }
        //Object* auxRefValue = newValue->type == NUMBER_ENTRY? newValue : NULL;
        updateType(useVar, header, writeSmvTypeTable, newValueBind, newValue->type, minmax, newValue);
        free(directiveValueBind);
    }
    // init casos
    // caso 1 :  init default + next ( seja de variável que só existe em t>0(pode também ser uma redefinição) (devem ignorar condições nesse init)
    // caso 2 : init unico, seja para variáveis que foram redefinidas em t = 0 ou declarações comuns (e agora com um next default para evitar estouro de estados)
    else{
        if((C_TIME && redef) || contextChange){

            conditionCube = formatCondtion(scope,1,1,newValueBind,directiveValueBind,1);

            createType(useVar, header, writeSmvTypeTable, defaultValueBind, newValue, newValue->type);
            createAssign(useVar, header, writeSmvTypeTable, defaultValueBind, conditionCube, INIT, defaultValueBind, 1,
                         0);
        }
        else{
            conditionCube = formatCondtion(scope,0,1,newValueBind,directiveValueBind,1);

            createType(useVar, header, writeSmvTypeTable, newValueBind, newValue, newValue->type);
            createAssign(useVar, header, writeSmvTypeTable, newValueBind, conditionCube, INIT, defaultValueBind, 1, 0);
            createDefaultNext(useVar,header,writeSmvTypeTable);
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

    char interTDS[ALOC_SIZE_LINE];
    sprintf(interTDS, SmvConversions[TDS_MODULE_NAME], declaredName);
    char* BIND_TDS = malloc(sizeof(ALOC_SIZE_LINE));
    strcpy(BIND_TDS,interTDS);
    return BIND_TDS;
}

/**
 * Cria um Header padrão que será escrito no arquivo SMV, sendo salvo até lá no controlador para futuras modificações
 * @param type o tipo do HEADER {MAIN = 1, AUTOMATA = 2, PORTS = 3, FUNCTION_SMV = 4}
 * @param name o nome processado anteriomente de maneira compor a declaração de módulo
 * @param varP o ponteiro de alocaçao de linhas de da parte var (util para controlar casos que o modulo nao venha a necessitar de alguma parte)
 * @param assignP o ponteiro de alocaçao de linhas de da parte var
 * @param transP o ponteiro de alocaçao de linhas de da parte trans
 * @param controller o controlador de ambiente para armazenar o header
 * @return o novo header alocado
 * @SideEffects: Aloaca um header que deve ser liberado como responsabilidade do chamador
 */
HeaderSmv* specHeader(smvtype type, char* name, int varP, int assignP, int transP, EnvController* controller){
    HeaderSmv* newHeader = createHeader(type,name,varP,assignP,transP);
    addNewHeader(controller,newHeader);
    selectBuffer(VAR,"VAR\n",newHeader,0);
    selectBuffer(ASSIGN,"ASSIGN\n",newHeader,0);
    return newHeader;
}

void validateTdsDeclaration(char* declarationName, EnvController* controller){
    TableEntry* expected_entry =  lookup(controller->originalPorts,declarationName);
    if(expected_entry){
        Object* info = expected_entry->val;
        int* isDeclared = info->values[0];
        if(!(*isDeclared)){
            controller->validPorts++;
            *isDeclared = 1;
        }
        else{
            controller->multiPortDeclartion = 1;
        }
    }
}

/***
 * Baseada na validação anterior (para tds's de time-components) verifica se deve criar init vazio ou init e next vazios.
 * Isso evita estouro de estados no nuXmv, considerando que caso esses não sejam especificados a TDS pode variar de forma
 * não deterministica.
 * @param newTDS
 * @param controller
 * @param initialIsInvalid
 * @param someIsValid
 */

void specAssignForInvalidTds(TDS* newTDS, EnvController* controller, int initialIsInvalid, int someIsValid){
    if(initialIsInvalid){
        // só o inicial é invalido
        createAssign("value", accessHeader(controller, PORTS, newTDS->SMV_REF),
                     accessSmvInfo(controller, PORTS, newTDS->AUX_REF), "NULL", NULL, INIT, NULL, 1, 0);
    }else{
        // nenhum é válido
        if(!someIsValid){
            createAssign("value", accessHeader(controller, PORTS, newTDS->SMV_REF),
                         accessSmvInfo(controller, PORTS, newTDS->AUX_REF), "NULL", NULL, INIT, NULL, 1, 0);
            createAssign("value", accessHeader(controller, PORTS, newTDS->SMV_REF),
                         accessSmvInfo(controller, PORTS, newTDS->AUX_REF), "value", NULL, NEXT, NULL, 1, 0);
        }
    }
}
/**
 * Valida se a nova TDS de time-component é válida, isto é, se nenhuma especificação para instantes exatos de tempo
 * violam I_TIME ~ F_TIME. Ou ainda, se a especificação da time-component é de um tempo já do passado
 * (gerando tornando um assign inútil e avaliações inúteis que podem ter efeitos indesejados no modelo e no interpretador)
 * @param encapsulatedTDS o objeto encapsulado da TDS, serve para notificar o usuário por meio do seu SYNTH_BIND em qual variável de TDS ocorreu a violação de validação
 * @param newTDS a nova TDS especificada
 * @param controller o controlador de ambiente
 * @param C_TIME o tempo corrente do intervalo
 * @param I_TIME o tempo inícial do intervalo
 * @param F_TIME o tempo final do intervalo
 * @return
 */

int validateTdsTimeList(Object* encapsulatedTDS, TDS* newTDS, EnvController*  controller, int C_TIME, int I_TIME, int F_TIME){
    int someIsValidToLazy = 0; // nenhum é valido = 0 , algum é valido = 1,  -1 a gente pode falar que algum é valido mas o primeiro é invalid (C_TIME = 0)
    int initialIsInvalid = 1;
    int i;
    Object * timeComponentList = newTDS->DATA_SPEC;
    for (i = 0; i < newTDS->DATA_SPEC->OBJECT_SIZE; i++){
        Object* timeComponent = (Object*) newTDS->DATA_SPEC->values[i];
        int time = *(int*) timeComponent->values[0];
        if(time < C_TIME){
            fprintf(stderr, "[WARNING] %s TDS's specification on time = %d was not evaluated. The specification was defined on a C_TIME  >= %d context!  \n",
                    encapsulatedTDS->SINTH_BIND,time,C_TIME);
            initialIsInvalid = time == 0?  1 : initialIsInvalid;
        }
        else if(time < I_TIME || time > F_TIME){
            fprintf(stderr, "[WARNING] %s TDS's specification on time = %d was not evaluated. The specification was defined out of the model time interval: %d ~ %d  \n",
                    encapsulatedTDS->SINTH_BIND,time,I_TIME,F_TIME);
        }
        else{
            if(!someIsValidToLazy){
                someIsValidToLazy = 1;
            }
            initialIsInvalid = time == 0?  0 : initialIsInvalid;
            newTDS->COMPONENT_TIMES[time] = i;
        }
    }
    specAssignForInvalidTds(newTDS,controller,initialIsInvalid,someIsValidToLazy);
//    if(initialIsInvalid && someIsValidToLazy){
//        someIsValidToLazy = -1; // senão vai ficar como 0 (então TODOS são invalidos)
//    }
    return someIsValidToLazy;
}

/**
 * Adiciona a nova TDS a lista de callby need das TDS (após o COMMIT de C_TIME). Note que, nos casos de Time list nem todas são sempre
 * adicionadas a lazy caso o instante de tempo especificado não seja alcançável (validação de time-list).
 * Vale ressaltar, que escolhemos epsular o tratamento da validaçao da Time list aqui para nao realizar a validaçao 2 vezes.
 * @param encapsulatedTDS o objeto da tds encapsulado
 * @param newTDS a TDS em si
 * @param controller o controlador de ambiente
 * @param C_TIME o tempo corrente
 * @param I_TIME o tempo inicial
 * @param F_TIME o tempo final
 * @SideEffects: Adiciona a TDS a lista de pendentes, e caso seja necessario algum tratamento sobre a data-list seus
 * init ou next sao declarados (linha alocada)
 */

void addTdsToLazyControl(Object* encapsulatedTDS, TDS* newTDS, EnvController* controller, int C_TIME, int I_TIME, int F_TIME){
    int addToLazy = newTDS->type != DATA_LIST? 1: validateTdsTimeList(encapsulatedTDS,newTDS,controller,C_TIME, I_TIME,F_TIME);
    if(addToLazy){
        controller->declaredPorts[controller->declaredPortsNumber] = newTDS;
        controller->declaredPortsNumber++;
    }
}


/**
 * Para uma TDS que possuia dependencias, cria os assigns apropriados.
 * @param newTDS a nova TDS
 * @param controller a
 * @param dependencies
 */
void addTdsRelationOnSmv(TDS* newTDS, EnvController* controller, TDS** dependencies){
    // adiciona informações de relacionamento no nuXmv (separar em método)
    int i;
    if(newTDS->delayed){
        if(newTDS->TOTAL_DEPENDENCIES_PT > 1){
            fprintf(stderr, "[WARNING] %s with delayed property and more than one dependency  \n",newTDS->name);
            exit(-1);
        }

        char refToTdsValue[ALOC_SIZE_LINE];
        sprintf(refToTdsValue,SmvConversions[TDS_VALUE_REF],newTDS->name);
        char defaultDelayedEvalCond[ALOC_SIZE_LINE];
        if(newTDS->I_INTERVAL > 0){
            sprintf(defaultDelayedEvalCond, SmvConversions[TDS_DELAYED_EXPR_NEXT_TIMEV], newTDS->I_INTERVAL, refToTdsValue,
                    dependencies[0]->name, newTDS->I_INTERVAL ,dependencies[0]->name, refToTdsValue);
        }
        else{
            sprintf(defaultDelayedEvalCond, SmvConversions[TDS_DELAYED_EXPR_NEXT], refToTdsValue,
                    dependencies[0]->name, dependencies[0]->name, refToTdsValue);
        }
        createAssign(refToTdsValue, accessHeader(controller, PORTS, 0),
                     accessSmvInfo(controller, PORTS, 0), "NULL", NULL,
                     INIT, NULL, 0, 0);
        createAssign(refToTdsValue, accessHeader(controller, PORTS, 0),
                     accessSmvInfo(controller, PORTS, 0), NULL, defaultDelayedEvalCond, NEXT,
                     refToTdsValue, 0, 0);
    }
    else if(newTDS->TOTAL_DEPENDENCIES_PT > 0){

        char refToTdsValue[ALOC_SIZE_LINE];
        sprintf(refToTdsValue,SmvConversions[TDS_VALUE_REF],newTDS->name);
        char defaultDelayedEvalCond[ALOC_SIZE_LINE];
        char refNextToTdsValue[strlen(newTDS->name)+14];
        sprintf(refNextToTdsValue,SmvConversions[NEXT],refToTdsValue);

        int limitedByTime = newTDS->I_INTERVAL > 0;



        TDS* firstDependence = dependencies[0];
        char refToDepTdsValue[strlen(firstDependence->name)+8];  // .value0
        sprintf(refToDepTdsValue,SmvConversions[TDS_VALUE_REF],firstDependence->name);
        char refNextToDepTdsValue[strlen(firstDependence->name)+14]; // next(.value)
        sprintf(refNextToDepTdsValue,SmvConversions[NEXT],refToDepTdsValue);
        if(newTDS->TOTAL_DEPENDENCIES_PT > 1){
            char resultExprInit[ALOC_SIZE_LINE*2];
            char resultExprNext[ALOC_SIZE_LINE*2+(newTDS->TOTAL_DEPENDENTS_PT*6)];

            TDS* secondDependence = dependencies[1];
            char refToDep2TdsValue[strlen(secondDependence->name)+8];
            sprintf(refToDep2TdsValue,SmvConversions[TDS_VALUE_REF],secondDependence->name);
            char refNextToDep2TdsValue[strlen(secondDependence->name)+14];
            sprintf(refNextToDep2TdsValue,SmvConversions[NEXT],refToDep2TdsValue);
            if(limitedByTime){
                sprintf(resultExprNext,SmvConversions[TDS_INPUT_EXPR_TIMEV],
                        "next(time)",newTDS->I_INTERVAL,refNextToDep2TdsValue, refNextToDepTdsValue,
                        "next(time)",newTDS->I_INTERVAL,refNextToDepTdsValue,refNextToDep2TdsValue);
                createAssign(refToTdsValue, accessHeader(controller, PORTS, 0),
                             accessSmvInfo(controller, PORTS, 0), "NULL", NULL,
                             INIT, NULL, 0, 0);
            }
            else{
                sprintf(resultExprInit,SmvConversions[TDS_INPUT_EXPR],
                        refToDep2TdsValue, refToDepTdsValue,
                        refToDepTdsValue,refToDep2TdsValue);
                sprintf(resultExprNext,SmvConversions[TDS_INPUT_EXPR],
                        refNextToDep2TdsValue, refNextToDepTdsValue,
                        refNextToDepTdsValue,refNextToDep2TdsValue);

                createAssign(refToTdsValue, accessHeader(controller, PORTS, 0), accessSmvInfo(controller, PORTS, 0),
                             NULL, resultExprInit, INIT, "NULL", 0, 0);
            }
            createAssign(refToTdsValue, accessHeader(controller, PORTS, 0), accessSmvInfo(controller, PORTS, 0),
                         NULL, resultExprNext, NEXT, "NULL", 0, 0);
        }
        else{
            if(limitedByTime){
                createAssign(refToTdsValue, accessHeader(controller, PORTS, 0),
                             accessSmvInfo(controller, PORTS, 0), "NULL", NULL,
                             INIT, NULL, 0, 0);

                char timeBind[ALOC_SIZE_LINE/2];
                sprintf(timeBind,"%d",newTDS->I_INTERVAL);
                char* evalByTimeNext = createConditionCube("next(time)",timeBind,">",refNextToDepTdsValue,1);
                createAssign(refToTdsValue, accessHeader(controller, PORTS, 0),
                             accessSmvInfo(controller, PORTS, 0), "NULL", evalByTimeNext,
                             NEXT, NULL, 1, 0);
            }
            else{
                createAssign(refToTdsValue, accessHeader(controller, PORTS, 0), accessSmvInfo(controller, PORTS, 0),
                             refToDepTdsValue, NULL, INIT, NULL, 0, 0);
                createAssign(refToTdsValue, accessHeader(controller, PORTS, 0), accessSmvInfo(controller, PORTS, 0),
                             refNextToDepTdsValue, NULL, NEXT, NULL, 0, 0);
            }
        }
        // limitado a duas dependencias
        /*
        for (i=0; i < newTDS->TOTAL_DEPENDENCIES_PT; i++){

            TDS* dependence = dependencies[i];
            char refToDepTdsValue[strlen(dependence->name)];
            sprintf(refToDepTdsValue,SmvConversions[TDS_VALUE_REF],newTDS->name);
            char refNextToDepTdsValue[strlen(dependence->name)+10];
            sprintf(refNextToDepTdsValue,SmvConversions[NEXT],newTDS->name);

            char* current = i+1 == newTDS->TOTAL_DEPENDENTS_PT? end: continuation;
            if(i == 0){
                sprintf(resultExprInit,SmvConversions[FirstcondType],,current);
                sprintf(resultExprNext,SmvConversions[FirstcondType],,current);
            }
            else{
                sprintf(resultExprInit,SmvConversions[InducondType],,current);
                sprintf(resultExprNext,SmvConversions[InducondType],,current);
            }
        }
         */
    }
}


/**
 * Adiciona a referência de declaração ao novo módulo TDS ao nuXmv (portsModule).
 * Caso a TDS seja relacional, o relacionamento será explicitado no portsModule.
 * @param moduleName
 * @param newEncapsulatedTDS
 * @param newTDS
 * @param controller
 * @param C_TIME
 * @param I_TIME
 * @param F_TIME
 */

void addTdsOnSmv(char* moduleName, Object * newEncapsulatedTDS, TDS* newTDS, EnvController* controller, int C_TIME, int I_TIME, int F_TIME){
    char  nameWithNoBreakL[ALOC_SIZE_LINE];
    nameWithNoBreakL[0] = '\0';
    char* declarationDetails = strstr(moduleName,"t");
    removeAfter(nameWithNoBreakL,declarationDetails,'\n');
    char* declarationName = newTDS->name? newTDS->name : newEncapsulatedTDS->SINTH_BIND;
    createType(declarationName,accessHeader(controller, PORTS, 0),
               accessSmvInfo(controller, PORTS, 0),nameWithNoBreakL,NULL,V_MODULE_DEC);
}

void preProcessTDS(Object* encapsulatedTDS, EnvController* controller, int C_TIME, int I_TIME, int F_TIME, TDS** SYNTH_DEP){
    TDS* SYNTH_TDS =  (TDS*)encapsulatedTDS->values[0];

    SYNTH_TDS->AUX_REF = controller->PORTS_INFO_CURRENT_SIZE;
    SYNTH_TDS->SMV_REF = controller->H_PORTS_CURRENT_SIZE;

    char moduleName[ALOC_SIZE_LINE];
    moduleName[0] = '\0';
    sprintf(moduleName,SmvConversions[MODULE_BREAK_LINE],encapsulatedTDS->SINTH_BIND);
    HeaderSmv* newTdsHeader = specHeader(PORTS, moduleName, 0, 0, -1, controller);

    STable* auxTable = createTable(SMV_PORTS, NULL, 0, 0, -1);
    addNewAuxInfo(controller,auxTable);

    createType("value",newTdsHeader,auxTable,"NULL, 0, 1",NULL,TDS_ENTRY);

    char* declarationName = SYNTH_TDS->name? SYNTH_TDS->name : encapsulatedTDS->SINTH_BIND;

    addTdsOnSmv(newTdsHeader->moduleName, encapsulatedTDS, SYNTH_TDS, controller, C_TIME, I_TIME, F_TIME);
    addTdsRelationOnSmv(SYNTH_TDS,controller,SYNTH_DEP);

    addTdsToLazyControl(encapsulatedTDS,SYNTH_TDS,controller,C_TIME,I_TIME,F_TIME);
    validateTdsDeclaration(declarationName,controller);
}

void specTDS(TDS* currentTDS, Object* lazyValue, int C_TIME, int I_TIME, EnvController *controller, STable *currentScope) {

    //HeaderSmv* newTdsHeader = specHeader(PORTS, encapsulatedTDS->SINTH_BIND, 0, 0, -1, controller);
    //TDS* SYNTH_TDS =  (TDS*)encapsulatedTDS->values[0];
    HeaderSmv *currentHeader = accessHeader(controller, PORTS, currentTDS->SMV_REF);
    STable *currentInfo = accessSmvInfo(controller, PORTS, currentTDS->AUX_REF);
    if (currentTDS->type == DATA_LIST) {
        if (C_TIME == I_TIME) {
            createAssign("value", currentHeader, currentInfo, lazyValue->SINTH_BIND, NULL, INIT, NULL, 1, 0);
        } else {
            char *conditionCube = NULL;
            char *directiveValueBind = formatDirective(C_TIME);
            if (lookup(currentInfo, "next(value)")) {
                conditionCube = formatCondtion(currentScope, 0, 0, lazyValue->SINTH_BIND, directiveValueBind, 0);
                updateAssign("value", currentHeader, currentInfo, lazyValue->SINTH_BIND, conditionCube, TDS_ENTRY, NEXT,
                             -1);
            } else {
                conditionCube = formatCondtion(currentScope, 0, 0, lazyValue->SINTH_BIND, directiveValueBind, 1);
                createAssign("value", currentHeader, currentInfo, lazyValue->SINTH_BIND, conditionCube, NEXT, "NULL",
                             1, 0);
            }
            updateType("value", currentHeader, currentInfo, lazyValue->SINTH_BIND, TDS_ENTRY, -1, lazyValue);
        }
    }
        // só tem que criar um init e next unicos ou somente atualizar o type-set (na verdade essa atualização deve ser feita sempre)
    else {
        // se essa TDS já foi avaliada, senão...
        if (currentTDS) {

        } else {

        }
    }
}
//    updateType("value",currentHeader,currentInfo,lazyValue->SINTH_BIND,TDS_ENTRY,-1,lazyValue);


//    char* valueString = "value";
//    char* defaultConditionEval = "NULL";
//    char* valueOnZero = "NULL"; // para caso a inicialização seja com NULL

    //Object* firstTimeComponent = (Object*) SYNTH_TDS->DATA_SPEC->values[0];



    // testando synth (de ponteiro de funçoes) (tenho que salvar tambem o caminho do programa) (o escopo e controller mudam mas isso nao)

    //Object* valueComponent = (Object*) timeComponent->values[1];
    //(n,scope,controllerSmv);


 /*
    if(SYNTH_TDS->type == DATA_LIST){

        char result[ALOC_SIZE_LINE];
        char* parcialResult = malloc(sizeof(ALOC_SIZE_LINE)*sizeof(char));

        char* limitCondition = NULL;
        int limitedByTime = 0;

        char* components[SYNTH_TDS->DATA_SPEC->OBJECT_SIZE]; // N strings. (no caso teste 3)

        int hasZeroTimeComponent = 0;
        int i;


        for (i = 0; i < SYNTH_TDS->DATA_SPEC->OBJECT_SIZE; ++i) {

            Object* timeComponent = (Object*) SYNTH_TDS->DATA_SPEC->values[1];
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
                if(i+1 == SYNTH_TDS->DATA_SPEC->OBJECT_SIZE){
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

void addTypeSetSmv(char* varName, int pos, int tam, char *newValueBind, int type, STable* writeSmvTypeTable)
{
    int usedSize = MAX_SIMPLE*MULTIPLIER_SIMPLE_HASH;
    int* typeSetHashMap = malloc(sizeof(int)*usedSize); // NOTE ! ele não inicia com zeros! Deve fazer limpeza.
    int i;
    for (i = 0; i < MAX_SIMPLE*MULTIPLIER_SIMPLE_HASH; i++) {
        typeSetHashMap[i] = 0;
    }
    if(type == TDS_ENTRY){
        int hashNULL = hash("NULL", usedSize);
        int hashZERO = hash("0", usedSize);
        int hashONE = hash("1", usedSize);
        typeSetHashMap[hashNULL] = 1;
        typeSetHashMap[hashZERO] = 1;
        typeSetHashMap[hashONE] = 1;
    }
    else{
        typeSetHashMap[hash(newValueBind, usedSize)] = 1;
    }
    void* po[] = {&pos, &tam,typeSetHashMap};
    addValue(varName, po, TYPE_SET, 3, 0, writeSmvTypeTable, 0);
}

void propagateTypeSet(TDS* dependence, TDS* dependant, EnvController* controller, int C_TIME){
    STable* auxTableDependant = accessSmvInfo(controller,PORTS,dependant->AUX_REF);

    HeaderSmv* headerDependant = accessHeader(controller,PORTS,dependant->SMV_REF);

    char rawValueBind[ALOC_SIZE_LINE/2];
    copyValueBind(dependence->DATA_TIME[C_TIME],rawValueBind,0,0,1);
    updateTypeSet(rawValueBind,"value",auxTableDependant,headerDependant);
}

void writeResultantHeaders(EnvController* controller, const char* path){
  
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


