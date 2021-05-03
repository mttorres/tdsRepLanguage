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
                          "%s = NULL : %s;\n\t\t%s = NULL : %s;",  "%s >= %d & %s = NULL : %s;\n\t\t%s >= %d & %s = NULL : %s;",
                          "MODULE %s\n" };



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
                int type, EnvController* controller)
{
    char* newType = malloc(sizeof(char)*ALOC_SIZE_LINE);
    int pos = header->VAR_POINTER;
    if(type == NUMBER_ENTRY || type == T_DIRECTIVE_ENTRY){
        TypeMinMax* SYNTH_MIN_MAX = (TypeMinMax*) newValue->type_smv_info;
        sprintf(newType, SmvConversions[INTERVAL_DEC], varName, SYNTH_MIN_MAX->min, SYNTH_MIN_MAX->max);
        char* auxDelim = strstr(newType,":");
        char* auxFim = strstr(auxDelim,"..");
        int pointIni = (auxDelim-newType+2);
        int pointEnd = ((auxFim-newType))-1;
        int tam = strlen(newType);

        void* po[] = {&pos, &tam, &pointIni, &pointEnd};
        addSmvInfoDeclaration(varName,po,WRITE_SMV_INFO,4,writeSmvTypeTable,newValue->type_smv_info);
    }
    else if(type == LOGICAL_ENTRY){
        sprintf(newType, SmvConversions[BOOLEAN_DEC], varName);
        int tam = strlen(newType);
        void* po[] = {&pos, &tam};
        addSmvInfoDeclaration(varName,po,WRITE_SMV_INFO,2,writeSmvTypeTable,newValue->type_smv_info);
        //addValue(varName, po, WRITE_SMV_INFO, 2, 0, writeSmvTypeTable, 0);

    }
    else if(type == LABEL_ENTRY || type == NULL_ENTRY)
    {
        sprintf(newType, SmvConversions[SET], varName, newValueBind);
        int tam = strlen(newType);
        void* po[] = {&pos, &tam};
        addSmvInfoDeclaration(varName,po,WRITE_SMV_INFO,2,writeSmvTypeTable,newValue->type_smv_info);
        //addTypeSetSmv(varName, pos, tam, newValueBind, type, writeSmvTypeTable, controller);
    }
    // cria uma variável que é instancia de modulo nuXmv
    else{
        sprintf(newType,SmvConversions[V_MODULE_DEC],varName,newValueBind);
        int tam = strlen(newType);
        void* po[] = {&pos, &tam};
        addSmvInfoDeclaration(varName,po,WRITE_SMV_INFO,2,writeSmvTypeTable,NULL);
        //addValue(varName, po, WRITE_SMV_INFO, 2, 0, writeSmvTypeTable, 0);
    }
    header->varBuffer[header->VAR_POINTER] = newType;
    header->VAR_POINTER += 1;
}

void unionTypeSetMinMaX(HeaderSmv* headerOriginal, TableEntry* entry_info_type_smv, TypeMinMax* other, EnvController* controller){
    TypeSet* original = entry_info_type_smv->val->type_smv_info;
    int i;
    int *pos = entry_info_type_smv->val->values[0];
    int *size = entry_info_type_smv->val->values[1];
    for(i = other->min; i <= other->max; i++){
        char numString[ALOC_SIZE_LINE/2];
        sprintf(numString,"%d",i);
        if(addElementToTypeSet(original,getTypeSetWordFromDict(numString,controller))){
            headerOriginal->varBuffer[*pos] = addParams(headerOriginal->varBuffer[*pos],numString,"{","}",1);
        }
    }
    *size = strlen(headerOriginal->varBuffer[*pos]);
}

void unionTypeSets(HeaderSmv* headerOriginal, TableEntry* entry_info_type_smv, TypeSet* other) {
    TypeSet* original = entry_info_type_smv->val->type_smv_info;
    char** hash_set_other = other->hash_set;
    int i;
    int *pos = entry_info_type_smv->val->values[0];
    int *size = entry_info_type_smv->val->values[1];
    for (i = 0; i <= other->lastIndex; i++) {
        if(hash_set_other[i]){
            if(addElementToTypeSet(original,hash_set_other[i])){
                headerOriginal->varBuffer[*pos] = addParams(headerOriginal->varBuffer[*pos],hash_set_other[i],"{","}",1);
            }
        }
    }
    *size = strlen(headerOriginal->varBuffer[*pos]);
}

/**
 * Realiza as operações de hash e atualização de um type-set
 * @param newValue o novo objeto que foi sintetizado e sua equivalencia em strin vai ser adicionada ao conjunto
 * @param varName o nome da variável
 * @param writeSmvTypeTable a tabela de simbolos auxiliar
 * @param header o header
 * @SideEffects: Altera as informações do objeto type-set recuperado da tabela auxiliar
 */
void updateTypeSet(Object* newValue, char* varName, STable* writeSmvTypeTable, HeaderSmv* header, EnvController* controller)
{
    if(!newValue->type_smv_info){
        fprintf(stderr,"[updateTypeSet] ERROR: No SMV_INFO!\n");
    }
    TableEntry* entryTypeSetInfo;
    entryTypeSetInfo =  lookup(writeSmvTypeTable,varName);
    if(newValue->type == NUMBER_ENTRY || newValue->type == WRITE_SMV_INFO){
        unionTypeSetMinMaX(header,entryTypeSetInfo,newValue->type_smv_info,controller);
    }
    else{
        if(newValue->type == LABEL_ENTRY || newValue->type == TYPE_SET){
            unionTypeSets(header,entryTypeSetInfo,newValue->type_smv_info);
        }
    }

    /*
    TableEntry* entryTypeSetInfo;
    entryTypeSetInfo =  lookup(writeSmvTypeTable,varName);
    int* pos = (int*) entryTypeSetInfo->val->values[0];
    int* size = (int*) entryTypeSetInfo->val->values[1];
    TypeSet* typeSet = entryTypeSetInfo->val->type_smv_info;
    addTypeSetWordToDict(newValue,controller);
    if(addElementToTypeSet(typeSet,getTypeSetWordFromDict(newValue,controller))){
        char* original = header->varBuffer[*pos];
        char* newTypeSet = addParams(original, newValue, "{", "}", 0);
        header->varBuffer[*pos] = newTypeSet;
        free(original);
        *size = strlen(newTypeSet);
    }
    */
}

void updateType(char *varName, HeaderSmv *header, STable *writeSmvTypeTable, const char *newValueBind, int type,
                void *newValue, EnvController* controller)
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
            TypeMinMax* minMaxEntry = entryPosType->val->type_smv_info;
            Object* newValueObject = type == T_DIRECTIVE_ENTRY? NULL : newValue;
            int newPointIni = 0;
            int newPointEnd = 0;
            int sizeNew = strlen(newValueBind);
            int minmax = -1;
            if(newValueObject && newValueObject->type_smv_info) {
                TypeMinMax* newMinMax = newValueObject->type_smv_info;
                if(minMaxEntry){
                    minmax = mergeTypeMinMax(minMaxEntry,newMinMax);
                }
                else{
                    fprintf(stderr,"[updateType] Error: %s entry does not contains MIN-MAX!\n",varName);
                    exit(-1);
                }
            }
            else{
                if(type != T_DIRECTIVE_ENTRY){
                    fprintf(stderr,"[updateType] Error: new value does not contains MIN-MAX!\n");
                    exit(-1);
                }
                minmax = changeMinMax(minMaxEntry,*(int*)newValue);
            }
            if(minmax == 2 || minmax == 0){
                // atualiza o inicio
                header->varBuffer[pos] = updateSubStringInterval(newValueBind, header->varBuffer[pos], sizeNew, pointIni, pointEnd, size, &newPointIni,
                                        &newPointEnd, 0);
                void* vpInEnd[] = {&newPointEnd};
                updateValue(varName, vpInEnd, WRITE_SMV_INFO, 1, 3, -1, writeSmvTypeTable, 0);
                pointEnd = newPointEnd;
            }
            if(minmax == 2 || minmax == 1){
                pointIni = pointEnd+3; // n..max;
                // nota! o size já está indexbased!
                pointEnd = header->varBuffer[pos][size-1] == '\n' ? size-3  : size-2; // max;\n (-1 do index based) - (-2 ou -1 dependendo do fim)
                //size = header->varBuffer[pos][size-1] == '\n' ? size-1 : size;
                header->varBuffer[pos]= updateSubStringInterval(newValueBind, header->varBuffer[pos], sizeNew, pointIni, pointEnd, size, &newPointIni,
                                        &newPointEnd, 0);
            }
            if(minmax != -1){
                //size = -1*((pointEnd-pointIni+1) - sizeNew) + size;
                size = strlen(header->varBuffer[pos]);
                void* vpSize[] = {&size};
                updateValue(varName, vpSize, WRITE_SMV_INFO, 1, 1, -1, writeSmvTypeTable, 0);
            }
        }
        else{
            printf("[updateType] WARNING: type of %s not declared on aux headers \n",varName);
            exit(-1);
        }
    }
    if(type == TDS_ENTRY || type == LABEL_ENTRY || type == NULL_ENTRY){
        updateTypeSet(newValue,varName,writeSmvTypeTable,header,controller);
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


void updateAssign(char *varName, HeaderSmv *header, STable *writeSmvTypeTable, char *newValue, char *condition, int type, int typeExpr)
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

    // verifica o tamanho possível após audan mça (o +1 é justamente porque se o intervalo é x-y = 0, o tamanho é 1,
    // ou seja torna como size based ao inves de index based)
    updated = header->assignBuffer[pos];

    // é um simples next(x)/init(x):= yyyyyyy;  -> substituir y's por expressão nova em newValue antes do delmitador (;)
    // os delmitadores podem variar, ex: em caso de next(time) o delimitador é (:)
    header->assignBuffer[pos] = updateSubStringInterval(newValue, updated, sizeNew, pointIni, pointEnd, size, &newPointInit, &newPointEnd, 1);

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
    char* activeName = malloc((sizeof(char)*ALOC_SIZE_LINE)+1);
    strcpy(activeName,useVar);
    return activeName;
}

/**
* Retorna um bind para um valor. Seja uma versão em String do mesmo ou ainda o nome de variável desse valor dentro do
* contexto do nuXmv.
* @param varName o nome da variável recuperado do escopo
* @param parentScope o escopo da entrada da tabela de simbolos
* @param expr o objeto associado
* @param index o indice caso seja uma estrutura de dados
* @param isDefault flag para avaliar para o valor default daquele tipo (0 inteiro, NULL : labels, FALSE : booleans)
* @param isSelf é usado para retornar o bind como sendo o próprio nome da variável (independente do estado, sem init ou next).
* Caso seja usado, esse sobrescreve o isDefault.
* @return retorna uma string alocada com o nome da variável dentro do contexto do nuXmv.
*/
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

Object *refCopyOfVariable(TableEntry *varLang, EnvController *controller) {
    char* useVar = NULL;
    // temos que usar escopo de VAR não o escopo atual de onde a chamada ocorre!
    // como nesse caso é necessário referênciar EXATAMENTE o nome da variável,
    int C_TIME = *(int*) lookup(varLang->parentScope, "C_TIME")->val->values[0];
    useVar = processActiveName(varLang->parentScope, varLang->name, 1, C_TIME, varLang->val->type);
    Object* copyRef = copyObject(varLang->val);
    if(useVar){
        if(copyRef->SINTH_BIND){
            free(copyRef->SINTH_BIND);
        }
        copyRef->SINTH_BIND =  malloc(sizeof(char)*(strlen(useVar)+ 1));
        strcpy(copyRef->SINTH_BIND,useVar);
    }

    // o nome da variável sem init/next
    char* varDeclarationNameRefSmv = processActiveName(varLang->parentScope, varLang->name, 1, 0, varLang->val->type);
    if(varDeclarationNameRefSmv){
        // devemos recuperar o type-min-max/type-set do escopo smv apropriado
        //(TODO) devemos adaptar isso para o funcionamento das funções usadas por tds.
        STable* smv_info = accessSmvInfo(controller,varLang->parentScope->type == GLOBAL || !varLang->parentScope->childOfFunction?
        MAIN : FUNCTION_SMV,0);
        Object* var_dec_info_smv = lookup(smv_info,varDeclarationNameRefSmv)->val;
        if(varLang->val->type != LOGICAL_ENTRY && !var_dec_info_smv->type_smv_info){
            fprintf(stderr,"[refCopyOfVariable] %s missing type SMV info!\n",varLang->name);
            exit(-1);
        }
        if (var_dec_info_smv->type == TYPE_SET) {
            copyRef->type_smv_info = copyTypeSet(var_dec_info_smv->type_smv_info);
        } else {
            copyRef->type_smv_info = copyTypeMinMax(var_dec_info_smv->type_smv_info);
        }
        free(varDeclarationNameRefSmv);
    }
    free(useVar);
    return copyRef;
}

/**
 * Dado que é possível criar ASSIGNS default, vale verificar se devemos sobrescrever esse após a variável
 * não ser mais não deterministica.
 * @param writeSmvTypeTable a tabela de simbolos auxiliar
 * @param statevarname o nome de estado da variável
 * @param headerSmv o header auxiliar
 * @return 1 caso exista um assign next default
 */
int checkFristDefaultAssign(STable* writeSmvTypeTable, char* statevarname, HeaderSmv* headerSmv){
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
           Object *newValue, int redef, int typeExpr, int C_TIME, EnvController *controller)
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

    //int defaultSelf = !varInit && scope->type == IF_BLOCK || scope->type == ELSE_BLOCK;
    int defaultSelf = !varInit;
    defaultValueBind = formatValueBind(varName, scope, newValue, 0, 1, defaultSelf);
    newValueBind = formatValueBind(varName, scope, newValue, 0, 0, 0);

    // next
    if(typeExpr){
        // criar next(useVar)
        char statevarname[ALOC_SIZE_LINE];
        sprintf(statevarname,SmvConversions[NEXT],useVar);
        //verifica se existe next(statevarname)
        directiveValueBind = formatNumeric(C_TIME);
        //defaultValueBind = formatValueBind(newValue,0,1); // vai ser só para o caso de ref de uma variável que foi atualizada dentro de um if (já existe fora do escopo atual)

        if(lookup(writeSmvTypeTable,statevarname)){
            if(checkFristDefaultAssign(writeSmvTypeTable, statevarname, header)){
                conditionCube = formatCondtion(scope,0,0,newValueBind,directiveValueBind,1);
                overwriteAssign(statevarname,varName,header,writeSmvTypeTable,newValueBind,conditionCube,defaultValueBind);
            }
            else{
                conditionCube = formatCondtion(scope,0,0,newValueBind,directiveValueBind,0);
                updateAssign(useVar, header, writeSmvTypeTable, newValueBind, conditionCube, newValue->type, NEXT);
            }
        }
        else{
            conditionCube = formatCondtion(scope,0,0,newValueBind,directiveValueBind,1);
            createAssign(useVar, header, writeSmvTypeTable, newValueBind, conditionCube, NEXT, NULL, 1, 0);
        }
        //Object* auxRefValue = newValue->type == NUMBER_ENTRY? newValue : NULL;
        updateType(useVar, header, writeSmvTypeTable, newValueBind, newValue->type, newValue, controller);
        free(directiveValueBind);
    }
    // init casos
    // caso 1 :  init default + next ( seja de variável que só existe em t>0(pode também ser uma redefinição) (devem ignorar condições nesse init)
    // caso 2 : init unico, seja para variáveis que foram redefinidas em t = 0 ou declarações comuns (e agora com um next default para evitar estouro de estados)
    else{
        if((C_TIME && redef) || contextChange){

            conditionCube = formatCondtion(scope,1,1,newValueBind,directiveValueBind,1);
            createType(useVar, header, writeSmvTypeTable, defaultValueBind, newValue, newValue->type, controller);
            createAssign(useVar, header, writeSmvTypeTable, defaultValueBind, conditionCube, INIT, defaultValueBind, 1,
                         0);
        }
        else{
            conditionCube = formatCondtion(scope,0,1,newValueBind,directiveValueBind,1);

            createType(useVar, header, writeSmvTypeTable, newValueBind, newValue, newValue->type, controller);
            createAssign(useVar, header, writeSmvTypeTable, newValueBind, conditionCube, INIT, defaultValueBind, 1, 0);
            createDefaultNext(useVar,header,writeSmvTypeTable);
        }
    }
    free(defaultValueBind);
    free(newValueBind);
    free(useVar);
}

void updateTime(HeaderSmv *main, STable *writeSmvTypeTable, char *newValueBind, int type, int typeExpr, int newValue)
{
    updateType("time", main, writeSmvTypeTable, newValueBind, type, &newValue, NULL);
    typeExpr ? updateAssign("time", main, writeSmvTypeTable, newValueBind, NULL, type, NEXT) :
    updateAssign("time", main, writeSmvTypeTable, newValueBind, NULL, type, INIT);
}

char* createReferenceTDS(char* declaredName){

    char interTDS[ALOC_SIZE_LINE];
    sprintf(interTDS, SmvConversions[TDS_MODULE_NAME], declaredName);
    char* BIND_TDS = malloc(sizeof(char)*ALOC_SIZE_LINE);
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
    HeaderSmv* newHeader = createHeader(type, name, varP, assignP, transP, 0);
    addNewHeader(controller,newHeader);
    selectBuffer(VAR,"VAR\n",newHeader,0);
    selectBuffer(ASSIGN,"ASSIGN\n",newHeader,0);
    return newHeader;
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
    for (i = 0; i < timeComponentList->OBJECT_SIZE; i++){
        Object* timeComponent = (Object*) timeComponentList->values[i];
        int time = *(int*) timeComponent->values[0];
        if(time < C_TIME){
            fprintf(stderr, "\n[WARNING] %s TDS's specification on time = %d was not evaluated.\nThe specification was defined on a C_TIME  >= %d context!  \n",
                    encapsulatedTDS->SINTH_BIND,time,C_TIME);
            initialIsInvalid = time == 0?  1 : initialIsInvalid;
        }
        else if(time < I_TIME || time > F_TIME){
            fprintf(stderr, "\n[WARNING] %s TDS's specification on time = %d was not evaluated.\nThe specification was defined out of the model time interval: %d ~ %d  \n",
                    encapsulatedTDS->SINTH_BIND,time,I_TIME,F_TIME);
        }
        else{
            if(!someIsValidToLazy){
                someIsValidToLazy = 1;
            }
            initialIsInvalid = time == I_TIME?  0 : initialIsInvalid;
            newTDS->COMPONENT_TIMES[time] = i;
        }
    }
    specAssignForInvalidTds(newTDS,controller,initialIsInvalid,someIsValidToLazy);
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
 * Para uma TDS que possuia dependencias, cria os assigns apropriados baseado nas diretivas (delayed, link e filter).
 * Essas são especificadas no portsModule para explicitar o relacionamento entre as TDS's.
 * @param newTDS a nova TDS
 * @param controller o controlador de ambiente
 */
void addTdsRelationOnSmv(TDS *newTDS, EnvController *controller, int I_TIME) {
    // adiciona informações de relacionamento no nuXmv (separar em método)
    TDS** dependencies = newTDS->linkedDependency;
    int limitedByTime = newTDS->I_INTERVAL > I_TIME;
    int i;
    if(newTDS->delayed){
        if(newTDS->TOTAL_DEPENDENCIES_PT > 1){
            fprintf(stderr, "[WARNING] %s with delayed property and more than one dependency  \n",newTDS->name);
            exit(-1);
        }

        char refToTdsValue[ALOC_SIZE_LINE];
        sprintf(refToTdsValue,SmvConversions[TDS_VALUE_REF],newTDS->name);
        char defaultDelayedEvalCond[ALOC_SIZE_LINE];
        if(newTDS->I_INTERVAL > I_TIME){
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

        if(newTDS->limitCondition){
            if(newTDS->TOTAL_DEPENDENCIES_PT > 1){
                fprintf(stderr, "[WARNING] %s with filter property and more than one dependency  \n",newTDS->name);
                exit(-1);
            }
            createAssign(refToTdsValue, accessHeader(controller, PORTS, 0),
                         accessSmvInfo(controller, PORTS, 0), "NULL", NULL,
                         INIT, NULL, 0, 1);
            createAssign(refToTdsValue, accessHeader(controller, PORTS, 0),
                         accessSmvInfo(controller, PORTS, 0), refToTdsValue, NULL,
                         NEXT, NULL, 0, 1);
        }
        else {
            char defaultDelayedEvalCond[ALOC_SIZE_LINE];
            char refNextToTdsValue[strlen(newTDS->name)+14];
            sprintf(refNextToTdsValue,SmvConversions[NEXT],refToTdsValue);
            TDS* firstDependence = dependencies[0];

            char refToDepTdsValue[strlen(firstDependence->name)+8];  // .value0
            sprintf(refToDepTdsValue,SmvConversions[TDS_VALUE_REF],firstDependence->name);
            char refNextToDepTdsValue[strlen(firstDependence->name)+14]; // next(.value)
            sprintf(refNextToDepTdsValue,SmvConversions[NEXT],refToDepTdsValue);

            if(newTDS->TOTAL_DEPENDENCIES_PT > 1){
                char resultExprInit[ALOC_SIZE_LINE*2];
                char resultExprNext[ALOC_SIZE_LINE*2+(newTDS->TOTAL_DEPENDENCIES_PT*6)];

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
                    char* evalByTimeNext = createConditionCube("next(time)",timeBind,">=",refNextToDepTdsValue,1);
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
        }
        // limitado a duas dependencias
    }
}



/**
 * Adiciona a referência de declaração ao novo módulo TDS ao nuXmv (portsModule).
 * Caso a TDS seja relacional, o relacionamento será explicitado no portsModule.
 * @param moduleName
 * @param newEncapsulatedTDS
 * @param newTDS
 * @param controller
 */

void addTdsOnSmv(char* moduleName, Object * newEncapsulatedTDS, TDS* newTDS, EnvController* controller){
    char  nameWithNoBreakL[ALOC_SIZE_LINE];
    nameWithNoBreakL[0] = '\0';
    char* declarationDetails = strstr(moduleName,"t");
    removeAfter(nameWithNoBreakL,declarationDetails,'\n');
    char* declarationName = newTDS->name? newTDS->name : newEncapsulatedTDS->SINTH_BIND;
    createType(declarationName,accessHeader(controller, PORTS, 0),
               accessSmvInfo(controller, PORTS, 0),nameWithNoBreakL,NULL,V_MODULE_DEC,controller);
    if(newTDS->type == MATH_EXPRESSION || newTDS->type == FUNCTION_APPLY){
        // deve criar um init e next BÁSICOS
        // deve-se instanciar a funçao dentro do modulo SMV dessa TDS. (se não fosse inline)
        if(newTDS->type == FUNCTION_APPLY){
            createAssign("value", accessHeader(controller, PORTS, newTDS->SMV_REF),
                         accessSmvInfo(controller, PORTS, newTDS->AUX_REF), "NULL", NULL,
                         INIT, NULL, 0, 1);
            createAssign("value", accessHeader(controller, PORTS, newTDS->SMV_REF),
                         accessSmvInfo(controller, PORTS, newTDS->AUX_REF), "value", NULL,
                         NEXT, NULL, 0, 1);
        }
    }
}

TypeSet *computeTypeSet(EnvController *controllerSmv, char *sint_value) {
    addTypeSetWordToDict(sint_value, controllerSmv);
    TypeSet* newTypeSet = createTypeSet(getTypeSetWordFromDict("NULL",controllerSmv));
    addElementToTypeSet(newTypeSet,getTypeSetWordFromDict(sint_value,controllerSmv));
    return newTypeSet;
}

/**
 * Para uma TDS processada, cria a declaração da variável "value" em seu módulo. Que tem o type-set default com os valores
 * NULL, 0, 1
 * @param tdsHeader o header (módulo) associado a tds
 * @param auxTable a tabela de simbolos auxiliar
 * @param controller o controlador de ambiente (útil para consultar o dicionário de type-sets)
 */
void createDefaultTypeSetTDS(HeaderSmv *tdsHeader, STable* auxTable, EnvController* controller) {
    TypeSet* tdsValueTypeSet = computeTypeSet(controller,"0");
    addElementToTypeSet(tdsValueTypeSet,getTypeSetWordFromDict("1",controller));
    char* newType = malloc(sizeof(char)*ALOC_SIZE_LINE);
    sprintf(newType, SmvConversions[SET], "value", "NULL, 0, 1");
    int pos = tdsHeader->VAR_POINTER;
    int tam = strlen(newType);
    void* po[] = {&pos, &tam};
    addSmvInfoDeclaration("value",po,TYPE_SET,2,auxTable,tdsValueTypeSet);
    tdsHeader->varBuffer[tdsHeader->VAR_POINTER] = newType;
    tdsHeader->VAR_POINTER += 1;
}



void preProcessTDS(Object* encapsulatedTDS, EnvController* controller, int C_TIME, int I_TIME, int F_TIME){
    TDS* SYNTH_TDS =  (TDS*)encapsulatedTDS->values[0];

    SYNTH_TDS->AUX_REF = controller->PORTS_INFO_CURRENT_SIZE;
    SYNTH_TDS->SMV_REF = controller->H_PORTS_CURRENT_SIZE;

    char moduleName[ALOC_SIZE_LINE];
    moduleName[0] = '\0';
    sprintf(moduleName,SmvConversions[MODULE_BREAK_LINE],encapsulatedTDS->SINTH_BIND);
    HeaderSmv* newTdsHeader = specHeader(PORTS, moduleName, 0, 0, -1, controller);

    STable* auxTable = createTable(SMV_PORTS, NULL, 0, 0, -1);
    addNewAuxInfo(controller,auxTable);

    createDefaultTypeSetTDS(newTdsHeader,auxTable,controller);

    char* declarationName = SYNTH_TDS->name? SYNTH_TDS->name : encapsulatedTDS->SINTH_BIND;

    addTdsOnSmv(newTdsHeader->moduleName, encapsulatedTDS, SYNTH_TDS, controller);
    if(SYNTH_TDS->type == TDS_DEPEN){
        addTdsRelationOnSmv(SYNTH_TDS, controller, I_TIME);
    }
    addTdsToLazyControl(encapsulatedTDS,SYNTH_TDS,controller,C_TIME,I_TIME,F_TIME);
    validateTdsDeclaration(declarationName,controller);
}

/**
 * Para tds's de avaliação "constante" atualiza seu init e next caso seja necessário
 * @param currentTDS a tds corrente
 * @param name o nome da variável de atualização
 * @param eval o resultado de avaliação (que será da condição ou atribuido direto)
 * @param stateId se é init ou next
 * @param I_TIME o tempo inicial usado para controle temporal
 * @param currentHeader o header usado
 * @param currentInfo a info usada
 * @param controller o controlador de ambiente
 */
void specTdsAssignOnRevaluation(TDS* currentTDS, char* name, char* eval, int stateId, int I_TIME,
                                HeaderSmv* currentHeader, STable* currentInfo, EnvController* controller){

    // validaçoes
    // next/init
    char stateVarNameTds[strlen(name)+2+4+1+1]; // () init ou next \0
    formatStateVar(name,stateId,stateVarNameTds);
    // condição em conjunto (filter + temporal) (se for necessario alguma delas for necessária)
    char* condition = NULL;
    char* temporalCondition = NULL; //  caso seja next, vamos atribuir com uma condição que considera tempo > initTDS
    char* cubeFinal = NULL;
    //deve então verificar se tem que criar assign ou overwrite para o state selecionado
    int hasDefault = checkFristDefaultAssign(currentInfo, stateVarNameTds, currentHeader);
    if(hasDefault){
        if(stateId == NEXT && currentTDS->I_INTERVAL != I_TIME){
            char* directiveValueBind = formatNumeric(currentTDS->I_INTERVAL);
            temporalCondition = createConditionCube("next(time)", directiveValueBind, ">=", NULL, 1);
        }
        if(currentTDS->limitCondition){
            condition = currentTDS->currenCondBindRef;
        }
        cubeFinal = temporalCondition && condition ?
                    createConditionCube(temporalCondition, condition, "&", eval, 1) :
                    temporalCondition ? createConditionCube(temporalCondition, "", "", eval,1) :
                    condition ? createConditionCube(condition, "", "", eval, 1) :
                    NULL;
        overwriteAssign(stateVarNameTds,name,currentHeader,currentInfo,eval,cubeFinal,"NULL");
        if(temporalCondition){
            free(temporalCondition);
        }
    }
}

void specTDS(TDS* currentTDS, Object* lazyValue, int C_TIME, int I_TIME, EnvController *controller, STable *currentScope) {

    HeaderSmv *currentHeader = !currentTDS->limitCondition?
            accessHeader(controller, PORTS, currentTDS->SMV_REF)
            : accessHeader(controller,PORTS,0);
    STable *currentInfo = !currentTDS->limitCondition?
            accessSmvInfo(controller, PORTS, currentTDS->AUX_REF)
            : accessSmvInfo(controller,PORTS,0);

    // só necessita de atualizações complexas para data-list as demais vão ser feitas só uma vez
    if (currentTDS->type == DATA_LIST) {
        if (C_TIME == I_TIME) {
            createAssign("value", currentHeader, currentInfo, lazyValue->SINTH_BIND, NULL, INIT, NULL, 1, 0);
        } else {
            char *conditionCube = NULL;
            char *directiveValueBind = formatNumeric(C_TIME);
            if (lookup(currentInfo, "next(value)")) {
                conditionCube = formatCondtion(currentScope, 0, 0, lazyValue->SINTH_BIND, directiveValueBind, 0);
                updateAssign("value", currentHeader, currentInfo, lazyValue->SINTH_BIND, conditionCube, TDS_ENTRY,
                             NEXT);
            } else {
                conditionCube = formatCondtion(currentScope, 0, 0, lazyValue->SINTH_BIND, directiveValueBind, 1);
                createAssign("value", currentHeader, currentInfo, lazyValue->SINTH_BIND, conditionCube, NEXT, "NULL",
                             1, 0);
            }
        }
    }
    // só tem que criar um init e next unicos ou somente atualizar o type-set (na verdade essa atualização deve ser feita sempre)
    else {
        int stateId = C_TIME > I_TIME? NEXT : INIT;
        char* evalBind = NULL;
        if(currentTDS->limitCondition){
            //{nome_da_TDS}.value
            char refToTdsValue[strlen(currentTDS->name)+1+5+1]; // .value\0
            formatTdsValueRef(currentTDS,refToTdsValue);
            char* evalRef = NULL;
            //{nome_da_dependencia_filtro}.value
            char depNameOnSmv[strlen(currentTDS->name)+1+5+1]; // .value\0
            formatTdsValueRef(currentTDS->linkedDependency[0],depNameOnSmv);
            //nome da dependencia com next ou o nome anterior
            char depNameOnSmvNext[strlen(depNameOnSmv)+2+4+1+1]; // () next \0
            if(stateId == NEXT){
                formatStateVar(depNameOnSmv,stateId,depNameOnSmvNext);
                evalBind = depNameOnSmvNext;
            }
            else{
                evalBind = depNameOnSmv;
            }
            specTdsAssignOnRevaluation(currentTDS, refToTdsValue, evalBind, stateId, I_TIME, currentHeader, currentInfo, controller);
        }
        else{
            specTdsAssignOnRevaluation(currentTDS, "value", lazyValue->SINTH_BIND, stateId, I_TIME, currentHeader, currentInfo, controller);
            //specTdsAssignOnRevaluation("value", I_TIME, C_TIME, controller);
        }
    }
    if(lazyValue){
        updateType("value",currentHeader,currentInfo,lazyValue->SINTH_BIND,TDS_ENTRY,lazyValue,controller);
    }
}


void propagateValueToTypeSet(TDS* currentTDS, TDS* dependency, EnvController* controller){
    STable* auxTdsDependant = accessSmvInfo(controller,PORTS,currentTDS->AUX_REF);
    HeaderSmv* headerDependant = accessHeader(controller,PORTS,currentTDS->AUX_REF);
    Object* newValue = lookup(accessSmvInfo(controller,PORTS,dependency->AUX_REF),"value")->val;
    updateTypeSet(newValue,"value",auxTdsDependant,headerDependant,controller);
}


void updateAllAutomataFilter(Object* condFilter, EnvController* controller){
    int i;
    if(controller->F_AUTOMATAS_CHANGE_POINTER && controller->modelHasFilter){
        //char* cond = "next(x) = 2"; // onde X já vai estar no parâmetro
        //char* condNeg = "!next(x) = 2"; // essas condições só estão marretadas ai para testes
        char condNeg[strlen(condFilter->SINTH_BIND)+2];
        createExprBind(condNeg,condFilter,NULL,"");
        for (i = 0; i < controller->F_AUTOMATAS_CHANGE_POINTER; i++) {
            int id_automata = controller->automatasToChange[i];
            HeaderSmv* automata = accessHeader(controller,AUTOMATA,id_automata);
            updateAutomataFilterCond(condNeg, automata,0);
            updateAutomataFilterCond(condFilter->SINTH_BIND, automata,1);
        }
    }
    controller->F_AUTOMATAS_CHANGE_POINTER = 0;
    // poderia até dar free nesses caras
}

void formatStateVar(char* varName, int stateId, char* refToUpdate){
    // init/next(nomeVariavel) (statevar)
    //char stateVarNameOnSmv[strlen(varName) + 14];
    sprintf(refToUpdate, SmvConversions[stateId], varName);
}

void formatTdsValueRef(TDS* currentTds, char* refToUpdate){
    //{nome_da_TDS}.value
    //char refToTdsValue[strlen(currentTds->name)+1+5+1]; // .value\0
    sprintf(refToUpdate,SmvConversions[TDS_VALUE_REF],currentTds->name);
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


