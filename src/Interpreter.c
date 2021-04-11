//
// Created by mateus on 28/10/2020.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../headers/Interpreter.h"

/*
 * Funções Auxiliares
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


void resolveDependencies(TDS* currentTDS, EnvController* controllerSmv, int C_TIME){
    //updateLimitCondition(currentTDS, C_TIME); (se a condição limite atual for true ele vai conseguir realizar
    // as operações dos métodos que vem a seguir, senão NULL vai ser retornado
    //Object* resolvedValueRef = NULL;
    if(currentTDS->delayed){
        resolveDelayedTdsDependencies(currentTDS,C_TIME);
    }
    else{
        resolveMergerTdsDependencies(currentTDS, C_TIME);
    }
    int i;
    // sempre deve resolver as dependencias de uma TDS já que a embora a dependente possa não ter recebido valores diretamente esse instante
    // o "tipo" do type-set da outra pode ter mudado (nuXmv é bem rigido a respeito disso)
    for (i = 0; i < currentTDS->TOTAL_DEPENDENCIES_PT; i++) {
        propagateValueToTypeSet(currentTDS, currentTDS->linkedDependency[i],controllerSmv);
    }
}

void resolveLazyTdsSpec(STable *currentScope, EnvController *controllerSmv, int C_TIME, int I_TIME, Node *PROGRAM_PATH, TDS *currentTDS) {

    Object*  lazyValue = eval(PROGRAM_PATH, currentScope, controllerSmv);
    if(lazyValue->aList){
        fprintf(stderr, "TDS VALIDATION ERROR: INCOMPATIBLE SPECIFICATION FOR TDS %s. DATA STRUCTURES ARE NOT ACCEPTED AS VALUES, ONLY SYMBOLIC VALUES.", currentTDS->name);
        exit(-1);
    }
    specTDS(currentTDS,lazyValue,C_TIME,I_TIME,controllerSmv,currentScope);
    addDataToTds(currentTDS,C_TIME,lazyValue);
}

void resolveTimeComponentSpec(STable *currentScope, EnvController *controllerSmv, int C_TIME, int I_TIME,
                              Node *PROGRAM_PATH, TDS *currentTDS, Object *timeComponent) {
    PROGRAM_PATH = (Node*) timeComponent->values[1];
    resolveLazyTdsSpec(currentScope, controllerSmv, C_TIME, I_TIME, PROGRAM_PATH, currentTDS);
}

void resolveTdsLazyEvaluation(STable *currentScope, EnvController *controllerSmv, int C_TIME) {
    int I_TIME = *(int*) lookup(currentScope, "I_TIME")->val->values[0];
    Node* PROGRAM_PATH = NULL;
    int i;
    for (i = 0; i < controllerSmv->declaredPortsNumber; i++) {
        // resolve call-by-need cada expressão ativa da TDS.
        TDS* currentTDS = controllerSmv->declaredPorts[i];
        controllerSmv->currentTDScontext = currentTDS;
        if(currentTDS->type == DATA_LIST){
            // eval de forma que ele deve saber qual componente temporal ele deve pegar
            if(currentTDS->COMPONENT_TIMES[C_TIME] != -1){
                Object* timeComponent = (Object*) currentTDS->DATA_SPEC->values[currentTDS->COMPONENT_TIMES[C_TIME]];
                resolveTimeComponentSpec(currentScope,controllerSmv,C_TIME,I_TIME,PROGRAM_PATH,currentTDS,timeComponent);
            }
        }else{
            // tds_dependentes já são resolvidas sempre (tds desse tipo deveria ser adicionada aos lazy?)
            if(currentTDS->type != TDS_DEPEN){
                // FAZ O PROCESSO
                resolveLazyTdsSpec(currentScope, controllerSmv, C_TIME, I_TIME, PROGRAM_PATH, currentTDS);
            }
            else{
                // senão deve verificar a dependência dessa TDS para atribuir o DATA_TIME também e resolver seu type-set
                resolveDependencies(currentTDS,controllerSmv,C_TIME);
            }

        }
    }
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
        resolveTdsLazyEvaluation(currentScope, controllerSmv, i);
    }
}

/*
 * Funções de avaliação
 */

Object* evalNUM(Node* n, STable* scope, EnvController* controllerSmv)
{
    int sint;
    sint = atoi(n->leafs[0]);
    //printf("[evalNUM] SINTH: %d \n",sint);
    void* ip[] = {&sint};
    Object* o = createObject(NUMBER_ENTRY, 1, ip, -1, n->leafs[0],  createMinMaxByOneValue(sint));
    return o;
}

Object* evalBOOL(Node* n, STable* scope, EnvController* controllerSmv)
{
    printf("[evalBOOL] \n");
    int sint;

    char* trueString = "TRUE";
    char* falseString = "FALSE";

    sint= strstr(n->leafs[0],"true")? 1 : 0;

    //printf("[evalBOOL] SINTH: %d \n",sint);

    void* bp[] = {&sint};

    Object* o = createObject(LOGICAL_ENTRY, 1, bp, -1, sint ? trueString : falseString, NULL);

    return o;
}


Object* evalSTRING(Node* n, STable* scope, EnvController* controllerSmv)
{
    //printf("[evalSTRING] \n");
    char* sint =  n->leafs[0];

    void* sp[] = {sint};

    //printf("[evalSTRING] SINTH: %s \n",sint);
    TypeSet *newTypeSet = computeTypeSet(controllerSmv, sint);
    Object* o = createObject(LABEL_ENTRY, 1, sp, -1, sint, newTypeSet);
    return o;
}


/*
	Pensar depois... encapsular NULL ?

*/

Object* evalNULL(Node* n, STable* scope, EnvController* controllerSmv)
{
    printf("[evalNULL] \n");
    // se eu interpretar como "NULL" do C mesmo podemos ter problemas(?) SIM NULL SERÁ UMA "LABEL" ESPECIAL
    char* sint =  n->leafs[0];
    // TALVEZ TENHAMOS QUE ADICIONAR AO NULL o 0 (para não ficar uma constante)
    Object* o = createObject(NULL_ENTRY, 0, NULL, -1, sint, computeTypeSet(controllerSmv,sint));
    return o;
}


Object* evalIDVAR(Node* n, STable* scope, EnvController* controllerSmv)
{
    printf("[evalIDVAR] \n");

    // VAI RECUPERAR UM OBJETO NA TABELA DE SIMBOLOS e então SUBIR COM ELE
    //(já que o tipo pode ser qualquer e a gente vai decidir o que fazer com ele ainda)


    TableEntry* entry = lookup(scope,n->leafs[0]);

    if(!entry)
    {
        fprintf(stderr, "%s NOT DECLARED!", n->leafs[0]);
        exit(-1);
    }
    else
    {
        // não pode retornar O MESMO PONTEIRO OU O OBJECT, senão teremos efeitos colaterais (ao mexer nesse mesmo usado por outra variavel alterariamos a anterior)
        //entry->val

        // DEVE-SE TER UMA COPIA DO OBJETO INTEIRO JÁ QUE NÃO É SIMPLESMENTE ALGO ATOMICO, tipo um número que depois vai ser jogado para cima e terá passos intermediarios
        // é um COPIA DE VARIAVEL ,

        // ou em casos de ED, uma referencia a uma ED já existente (a ideia é fazer a mesma coisa com TDS ao invés de copiar por razões óbvias)

        // precisamos alocar um vetor de ponteiros para void ou um ponteiro único (já que pode variar, o ideal é por que a função sempre retorne void**)

        // criar MÉTODO DE COPIA DE OBJETO (variavel)

        // retorna a referência (ai pode sim ter colaterais) (não permite "passagem de referência" gerar conversão no nuXmv (não existe)
        if(entry->val->type == TDS_ENTRY || entry->val->OBJECT_SIZE > 1 || entry->val->type == NULL_ENTRY)
        {
            if(entry->val->type != NULL_ENTRY && controllerSmv->currentTDScontext){
                fprintf(stderr, "TDS VALIDATION ERROR: INCOMPATIBLE SPECIFICATION FOR TDS %s. DATA STRUCTURES ARE NOT ACCEPTED AS VALUES, ONLY SYMBOLIC VALUES.", controllerSmv->currentTDScontext->name);
                exit(-1);
            }
            return entry->val;
        }
        else
        {
            if(controllerSmv->currentTDScontext) {
                if (entry->val->type == LOGICAL_ENTRY) {
                    fprintf(stderr,
                            "TDS VALIDATION ERROR: INCOMPATIBLE SPECIFICATION FOR TDS %s. ONLY SYMBOLIC VALUES ARE ACCEPTED, %s is a BOOLEAN value",
                            controllerSmv->currentTDScontext->name, entry->name);
                    // poderia ter um cast implicito também...
                    exit(-1);
                }
            }
            Object *copy = refCopyOfVariable(entry, controllerSmv);
            if (controllerSmv->currentTDScontext) {
                    addParamToTds(controllerSmv, entry->name, controllerSmv->currentTDScontext);
                    // encapsular em método depois
                    //int C_TIME = *(int *) lookup(scope, "C_TIME")->val->values[0];
                    //addToTdsWatchList(controllerSmv->currentTDScontext,entry->name,C_TIME);
            }
            return copy;
        }
    }
}


Object* evalTIME_DIRECTIVE(Node* n, STable* scope, EnvController* controllerSmv)
{
    //printf("[evalTIME_DIRECTIVE] \n");
    // TODO mudar a referencia para não ser time sempre (#68)
    TableEntry* entry = lookup(scope,n->leafs[0]);

    // teoricamente é impossível uma time Directive não estar na tabela mas é só um check
    if(!entry)
    {
        fprintf(stderr, "%s NOT DECLARED!", n->leafs[0]);
        exit(-1);
    }
    else
    {
        // retorna cópia numérica das TIME_DIRECTIVES (elas SÃO UNICAS NO CÓDIGO, só alteradas mas não copiadas )
        return createObject(NUMBER_ENTRY, 1, entry->val->values, -1, "time", NULL);
    }
}

Object* evalDataV(Node* n, STable* scope, EnvController* controllerSmv){/*notused*/}

Object* evalPARAMS_CALL(Node* n, STable* scope, EnvController* controllerSmv)
{
    printf("[evalPARAMS_CALL] \n");
}

Object* evalPARAMS(Node* n, STable* scope, EnvController* controllerSmv)
{
    printf("[evalPARAMS_CALL] \n");
}


Object* evalPLUS(Node* n, STable* scope, EnvController* controller)
{
    Object* o1 = eval(n->children[0],scope,controller);
    Object* o2 = eval(n->children[1],scope,controller);

    if(!o1->aList && !o2->aList && o1->type == NUMBER_ENTRY && o2->type == o1->type){
        int r;
        void* rp[] = {&r};
        r =  (*(int*)o1->values[0]) + (*(int*)o2->values[0]);
        char resultingBind[strlen(o1->SINTH_BIND)+strlen(o2->SINTH_BIND) +4]; // + espaco + espaco + '+' + 1
        createExprBind(resultingBind, o1, o2, "+");
        Object* SUM_OBJECT =  createObject(NUMBER_ENTRY, 1, rp, -1,
                                           resultingBind, computeMinMaxSum(o1->type_smv_info,o2->type_smv_info));
        letgoObject(o1);
        letgoObject(o2);
        return SUM_OBJECT;
    }
    else if(!o1->aList && !o2->aList && o1->type == LABEL_ENTRY && o2->type == o1->type){
        char* str1 = o1->values[0];
        char* str2 = o2->values[0];
        char r[strlen(str1)+ strlen(str2)];
        r[0] = '\0';
        char* refToEnd = customCat(r,str1,0,0);
        customCat(refToEnd,str2,0,0);
        void* rp[] = {r};
        Object* CONCAT_OBJECT = createObject(LABEL_ENTRY, 1, rp,
                                             -1, r,computeTypeSet(controller,r));
        letgoObject(o1);
        letgoObject(o2);
        return CONCAT_OBJECT;
    }
    else{
        fprintf(stderr, "INCOMPATIBLE OPERANDS %s and %s FOR THE (+) OPERATION!",o1->SINTH_BIND,o2->SINTH_BIND);
        exit(-1);
    }
}

Object* evalMINUS(Node* n, STable* scope, EnvController* controller)
{
    Object* o1 = eval(n->children[0],scope,controller);
    Object* o2 = NULL;
    if(n->nchild > 1){
        o2 = eval(n->children[1],scope,controller);
    }
    if(o1->aList || (o2 && o2->aList) || (o1->type != NUMBER_ENTRY || (o2 && o2->type != NUMBER_ENTRY)))
    {
        if(o2){
            fprintf(stderr, "INCOMPATIBLE OPERANDS %s and %s FOR THE (-) OPERATION!",o1->SINTH_BIND,o2->SINTH_BIND);
            exit(-1);
        }
        else{
            fprintf(stderr, "INCOMPATIBLE OPERAND %s FOR THE (-) OPERATION!",o1->SINTH_BIND);
        }
        exit(-1);
    }
    int r;
    r =  o2 == NULL? (-1)*(*(int*) o1->values[0]) : (*(int*)o1->values[0]) - (*(int*)o2->values[0]);
    void* rp[] = {&r};
    char resultingBind[o2? strlen(o1->SINTH_BIND)+strlen(o2->SINTH_BIND) +4: strlen(o1->SINTH_BIND) +2]; // + espaco + espaco + '-' + 1 ou  '-' + +1
    createExprBind(resultingBind, o1, o2, "-");
    Object* MINUS_OBJECT =  createObject(NUMBER_ENTRY, 1, rp, -1, resultingBind,
                                         o2? computeMinMaxSub(o1->type_smv_info,o2->type_smv_info) : computeMinMaxNeg(o1->type_smv_info));
    letgoObject(o1);
    if(o2){
        letgoObject(o2);
    }
    return MINUS_OBJECT;
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

Object* evalNOT(Node* n, STable* scope, EnvController* controller)
{
    Object* o = eval(n->children[0],scope,controller);
    return notObjectOperation(o);
}

Object* evalMULTI(Node* n, STable* scope, EnvController* controller)
{
    Object* o1 = eval(n->children[0],scope,controller);
    Object* o2 = eval(n->children[1],scope,controller);

    if(!o1->aList && !o2->aList && o1->type == NUMBER_ENTRY && o2->type == o1->type){
        int r;
        void* rp[] = {&r};
        r =  (*(int*)o1->values[0]) * (*(int*)o2->values[0]);
        char resultingBind[strlen(o1->SINTH_BIND)+strlen(o2->SINTH_BIND) +4]; // + espaco + espaco + '*' + 1
        createExprBind(resultingBind, o1, o2, "*");
        Object* MULT_OBJECT = createObject(NUMBER_ENTRY, 1, rp, -1, resultingBind,
                                           computeMinMaxMul(o1->type_smv_info,o2->type_smv_info));
        letgoObject(o1);
        letgoObject(o2);
        return MULT_OBJECT;
    }
    fprintf(stderr, "INCOMPATIBLE OPERANDS %s and %s FOR THE (*) OPERATION!",o1->SINTH_BIND,o2->SINTH_BIND);
    exit(-1);
}

Object* evalDIV(Node* n, STable* scope, EnvController* controller)
{
    Object* o1 = eval(n->children[0],scope,controller);
    Object* o2 = eval(n->children[1],scope,controller);
    int isDivision = n->leafs[0][0] == DIVIDE;
    char* op = isDivision ? "/" : "MOD";

    if(!o1->aList && !o2->aList && o1->type == NUMBER_ENTRY && o2->type == o1->type){
        int r;
        void* rp[] = {&r};
        int SYNTH_O2 = *(int*)o2->values[0];
        if(!SYNTH_O2){
            fprintf(stderr, "CANNOT DIVIDE %s BY ZERO!",o1->SINTH_BIND);
            exit(-1);
        }
        int aloca = isDivision ? 4 : 7;
        r =  isDivision? (*(int*)o1->values[0]) / (SYNTH_O2) : (*(int*)o1->values[0]) % (SYNTH_O2);
        char resultingBind[strlen(o1->SINTH_BIND)+strlen(o2->SINTH_BIND) +aloca]; // + espaco + espaco + '/' + 1
        createExprBind(resultingBind, o1, o2, op);
        Object* DIV_OBJECT = createObject(NUMBER_ENTRY, 1, rp, -1, resultingBind,
                                          computeMinMaxDiv(o1->type_smv_info,o2->type_smv_info));
        letgoObject(o1);
        letgoObject(o2);
        return DIV_OBJECT;
    }
    fprintf(stderr, "INCOMPATIBLE OPERANDS %s and %s FOR THE (%s) OPERATION!",o1->SINTH_BIND,o2->SINTH_BIND,op);
    exit(-1);
}

Object * evalLE(Node* n, STable* scope, EnvController* controller){
    Object* o1 = eval(n->children[0],scope,controller);
    Object* o2 = eval(n->children[1],scope,controller);

    int r;
    void* rp[] = {&r};
    if(o1->type == NUMBER_ENTRY && o2->type == o1->type){
        r =  (*(int*)o1->values[0]) <= (*(int*)o2->values[0]);
        char resultingBind[strlen(o1->SINTH_BIND)+strlen(o2->SINTH_BIND) +5]; // + espaco + espaco + '<=' + 1
        createExprBind(resultingBind, o1, o2, "<=");
        letgoObject(o1);
        letgoObject(o2);
        return createObject(LOGICAL_ENTRY, 1, rp, -1, resultingBind, NULL);
    }
    else if(o1->type != TDS_ENTRY && o1->type != LOGICAL_ENTRY && o2->type != TDS_ENTRY && o2->type != LOGICAL_ENTRY){
        // compara tamanhos
        r =  o1->OBJECT_SIZE <= o2->OBJECT_SIZE;
        char resultingBind[300];
        sprintf(resultingBind,"%d <= %d",o1->OBJECT_SIZE,o2->OBJECT_SIZE);
        if(!o1->aList){
            letgoObject(o1);
        }
        if(!o2->aList){
            letgoObject(o2);
        }
        return createObject(LOGICAL_ENTRY, 1, rp, -1, resultingBind, NULL);
    }
    else{
        fprintf(stderr, "INCOMPATIBLE OPERANDS %s and %s FOR THE (<=) OPERATION!",o1->SINTH_BIND,o2->SINTH_BIND);
        exit(-1);
    }
}


Object * evalGE(Node* n, STable* scope, EnvController* controller){
    Object* o1 = eval(n->children[0],scope,controller);
    Object* o2 = eval(n->children[1],scope,controller);

    int r;
    void* rp[] = {&r};
    if(o1->type == NUMBER_ENTRY && o2->type == o1->type){
        r =  (*(int*)o1->values[0]) >= (*(int*)o2->values[0]);
        char resultingBind[strlen(o1->SINTH_BIND)+strlen(o2->SINTH_BIND) +5]; // + espaco + espaco + '<=' + 1
        createExprBind(resultingBind, o1, o2, ">=");
        letgoObject(o1);
        letgoObject(o2);
        return createObject(LOGICAL_ENTRY, 1, rp, -1, resultingBind, NULL);
    }
    else if(o1->type != TDS_ENTRY && o1->type != LOGICAL_ENTRY && o2->type != TDS_ENTRY && o2->type != LOGICAL_ENTRY){
        // compara tamanhos
        r =  o1->OBJECT_SIZE >= o2->OBJECT_SIZE;
        char resultingBind[300];
        sprintf(resultingBind,"%d >= %d",o1->OBJECT_SIZE,o2->OBJECT_SIZE);
        if(!o1->aList){
            letgoObject(o1);
        }
        if(!o2->aList){
            letgoObject(o2);
        }
        return createObject(LOGICAL_ENTRY, 1, rp, -1, resultingBind, NULL);
    }
    else{
        fprintf(stderr, "INCOMPATIBLE OPERANDS %s and %s FOR THE (>=) OPERATION!",o1->SINTH_BIND,o2->SINTH_BIND);
        exit(-1);
    }
}

Object * evalLT(Node* n, STable* scope, EnvController* controller){
    Object* o1 = eval(n->children[0],scope,controller);
    Object* o2 = eval(n->children[1],scope,controller);

    int r;
    void* rp[] = {&r};
    if(o1->type == NUMBER_ENTRY && o2->type == o1->type){
        r =  (*(int*)o1->values[0]) < (*(int*)o2->values[0]);
        char resultingBind[strlen(o1->SINTH_BIND)+strlen(o2->SINTH_BIND) +5]; // + espaco + espaco + '<=' + 1
        createExprBind(resultingBind, o1, o2, "<");
        letgoObject(o1);
        letgoObject(o2);
        return createObject(LOGICAL_ENTRY, 1, rp, -1, resultingBind, NULL);
    }
    else if(o1->type != TDS_ENTRY && o1->type != LOGICAL_ENTRY && o2->type != TDS_ENTRY && o2->type != LOGICAL_ENTRY){
        // compara tamanhos
        r =  o1->OBJECT_SIZE < o2->OBJECT_SIZE;
        char resultingBind[300];
        sprintf(resultingBind,"%d < %d",o1->OBJECT_SIZE,o2->OBJECT_SIZE);
        if(!o1->aList){
            letgoObject(o1);
        }
        if(!o2->aList){
            letgoObject(o2);
        }
        return createObject(LOGICAL_ENTRY, 1, rp, -1, resultingBind, NULL);
    }
    else{
        fprintf(stderr, "INCOMPATIBLE OPERANDS %s and %s FOR THE (<) OPERATION!",o1->SINTH_BIND,o2->SINTH_BIND);
        exit(-1);
    }
}

Object * evalGT(Node* n, STable* scope, EnvController* controller){
    Object* o1 = eval(n->children[0],scope,controller);
    Object* o2 = eval(n->children[1],scope,controller);

    int r;
    void* rp[] = {&r};
    if(o1->type == NUMBER_ENTRY && o2->type == o1->type){
        r =  (*(int*)o1->values[0]) > (*(int*)o2->values[0]);
        char resultingBind[strlen(o1->SINTH_BIND)+strlen(o2->SINTH_BIND) +5]; // + espaco + espaco + '<=' + 1
        createExprBind(resultingBind, o1, o2, ">");
        letgoObject(o1);
        letgoObject(o2);
        return createObject(LOGICAL_ENTRY, 1, rp, -1, resultingBind, NULL);
    }
    else if(o1->type != TDS_ENTRY && o1->type != LOGICAL_ENTRY && o2->type != TDS_ENTRY && o2->type != LOGICAL_ENTRY){
        // compara tamanhos
        r =  o1->OBJECT_SIZE > o2->OBJECT_SIZE;
        char resultingBind[300];
        sprintf(resultingBind,"%d > %d",o1->OBJECT_SIZE,o2->OBJECT_SIZE);
        if(!o1->aList){
            letgoObject(o1);
        }
        if(!o2->aList){
            letgoObject(o2);
        }
        return createObject(LOGICAL_ENTRY, 1, rp, -1, resultingBind, NULL);
    }
    else{
        fprintf(stderr, "INCOMPATIBLE OPERANDS %s and %s FOR THE (>) OPERATION!",o1->SINTH_BIND,o2->SINTH_BIND);
        exit(-1);
    }
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

Object* evalEQUAL(Node* n, STable* scope, EnvController* controller){
    Object* o1 = eval(n->children[0],scope,controller);
    Object* o2 = eval(n->children[1],scope,controller);
    int r = comparator(o1,o2,"=");
    void* rp[] = {&r};
    char resultingBind[strlen(o1->SINTH_BIND)+strlen(o2->SINTH_BIND) +5];
    createExprBind(resultingBind, o1, o2, "=");
    return createObject(LOGICAL_ENTRY, 1, rp, -1, resultingBind, NULL);
}

Object* evalNEQUAL(Node* n, STable* scope, EnvController* controller){
    Object* o1 = eval(n->children[0],scope,controller);
    Object* o2 = eval(n->children[1],scope,controller);
    int r = !comparator(o1,o2,"!=");
    void* rp[] = {&r};
    char resultingBind[strlen(o1->SINTH_BIND)+strlen(o2->SINTH_BIND) +5];
    createExprBind(resultingBind, o1, o2, "!=");
    return createObject(LOGICAL_ENTRY, 1, rp, -1, resultingBind, NULL);
}


Object* evalAND(Node* n, STable* scope, EnvController* controller)
{
    Object* o1 = eval(n->children[0],scope,controller);
    Object* o2 = eval(n->children[1],scope,controller);

    if(!o1->aList && !o2->aList && o1->type == LOGICAL_ENTRY && o2->type == o1->type){
        int r;
        void* rp[] = {&r};
        r =  (*(int*)o1->values[0]) && (*(int*)o2->values[0]);
        char resultingBind[strlen(o1->SINTH_BIND)+strlen(o2->SINTH_BIND) +4]; // + espaco + espaco + '+' + 1
        createExprBind(resultingBind, o1, o2, "&");
        letgoObject(o1);
        letgoObject(o2);
        return createObject(LOGICAL_ENTRY, 1, rp, -1, resultingBind, NULL);
    }
    else{
        fprintf(stderr, "INCOMPATIBLE OPERANDS %s and %s FOR THE (and) OPERATION!",o1->SINTH_BIND,o2->SINTH_BIND);
        exit(-1);
    }
}

Object* evalOR(Node* n, STable* scope, EnvController* controller)
{
    Object* o1 = eval(n->children[0],scope,controller);
    Object* o2 = eval(n->children[1],scope,controller);

    if(!o1->aList && !o2->aList && o1->type == LOGICAL_ENTRY && o2->type == o1->type){
        int r;
        void* rp[] = {&r};
        r =  (*(int*)o1->values[0]) || (*(int*)o2->values[0]);
        char resultingBind[strlen(o1->SINTH_BIND)+strlen(o2->SINTH_BIND) +4]; // + espaco + espaco + '+' + 1
        createExprBind(resultingBind, o1, o2, "|");
        letgoObject(o1);
        letgoObject(o2);
        return createObject(LOGICAL_ENTRY, 1, rp, -1, resultingBind, NULL);
    }
    else{
        fprintf(stderr, "INCOMPATIBLE OPERANDS %s and %s FOR THE (or) OPERATION!",o1->SINTH_BIND,o2->SINTH_BIND);
        exit(-1);
    }
}

/**
 * Implementa implicação lógica
 * @param a primeiro predicado
 * @param b segundo predicado
 * @return um resultado 0 ou 1
 */
_Bool IMP(_Bool a, _Bool b){ return !a || b; }

/**
 * Implementa biimplicação lógica
 * @param a primeiro predicado
 * @param b segundo predicado
 * @return um resultado 0 ou 1
 */
_Bool BIMP(_Bool a, _Bool b){ return (a && b) || (!a && !b); }

Object* evalIMP(Node* n, STable* scope, EnvController* controller)
{
    Object* o1 = eval(n->children[0],scope,controller);
    Object* o2 = eval(n->children[1],scope,controller);

    if(!o1->aList && !o2->aList && o1->type == LOGICAL_ENTRY && o2->type == o1->type){
        int r;
        void* rp[] = {&r};
        r =  IMP((*(int*)o1->values[0]) , (*(int*)o2->values[0]));
        char resultingBind[strlen(o1->SINTH_BIND)+strlen(o2->SINTH_BIND) +5]; // + espaco + espaco + '->' + 1
        createExprBind(resultingBind, o1, o2, "->");
        letgoObject(o1);
        letgoObject(o2);
        return createObject(LOGICAL_ENTRY, 1, rp, -1, resultingBind, NULL);
    }
    else{
        fprintf(stderr, "INCOMPATIBLE OPERANDS %s and %s FOR THE (->) OPERATION!",o1->SINTH_BIND,o2->SINTH_BIND);
        exit(-1);
    }
}


Object* evalBIMP(Node* n, STable* scope, EnvController* controller)
{
    Object* o1 = eval(n->children[0],scope,controller);
    Object* o2 = eval(n->children[1],scope,controller);

    if(!o1->aList && !o2->aList && o1->type == LOGICAL_ENTRY && o2->type == o1->type){
        int r;
        void* rp[] = {&r};
        r =  BIMP((*(int*)o1->values[0]) , (*(int*)o2->values[0]));
        char resultingBind[strlen(o1->SINTH_BIND)+strlen(o2->SINTH_BIND) +6]; // + espaco + espaco + '+' + 1
        createExprBind(resultingBind, o1, o2, "<->");
        letgoObject(o1);
        letgoObject(o2);
        return createObject(LOGICAL_ENTRY, 1, rp, -1, resultingBind, NULL);
    }
    else{
        fprintf(stderr, "INCOMPATIBLE OPERANDS %s and %s FOR THE (<->) OPERATION!",o1->SINTH_BIND,o2->SINTH_BIND);
        exit(-1);
    }
}

Object* evalPRI_EXPR(Node* n, STable* scope, EnvController* controllerSmv)
{
    Object* pri = eval(n->children[0],scope,controllerSmv);
    char* NEW_BIND = malloc(sizeof(char)*(strlen(pri->SINTH_BIND)+3)); // ( ) e \0
    NEW_BIND[0] = '\0';
    char* refToNew = NEW_BIND;
    refToNew = customCat(refToNew,"(",0,0);
    refToNew = customCat(refToNew,pri->SINTH_BIND,0,0);
    customCat(refToNew,")",0,0);
    free(pri->SINTH_BIND);
    pri->SINTH_BIND = NEW_BIND;
    return pri;
}

Object* evalEXPR(Node* n, STable* scope, EnvController* controllerSmv)
{
    return eval(n->children[0],scope,controllerSmv);
}


Object* evalProp(Node* fatherRef, Node* n, STable* scope, EnvController* controllerSmv){/*notused*/}
Object* evalAC_V(Node* n, STable* scope, EnvController* controllerSmv){/*notused*/}
Object* evalV_PROP(Node* n, STable* scope, EnvController* controllerSmv){/*notused*/}
Object* evalADD_V(Node* n, STable* scope, EnvController* controllerSmv){/*notused*/}
Object* evalADD_V_PROP(Node* n, STable* scope, EnvController* controllerSmv){/*notused*/}

Object* evalV_PROP_TDS(Node* n, STable* scope, EnvController* controllerSmv){
    printf("[evalV_PROP_TDS] \n");
}

Object * evalDEFINE_INTERVAL(Node* n, STable* scope, EnvController* controllerSmv){

    int I_TIME;
    int* ptitime = NULL;
    int F_TIME;
    int* ptftime = NULL;
    // definiu inicio e fim
    if(n->nleafs == 3)
    {
        I_TIME = atoi(n->leafs[0]);
        F_TIME = atoi(n->leafs[2]);
        ptitime = &I_TIME;
        ptftime = &F_TIME;
    }
    else{
        char betSeparator = '~';
        if(n->leafs[0][0] == betSeparator){
            F_TIME = atoi(n->leafs[1]);
            ptftime = &F_TIME;
        }
        else{
            I_TIME = atoi(n->leafs[0]);
            ptitime = &I_TIME;
        }
    }
    if(  (ptitime != NULL && ptftime != NULL && I_TIME > F_TIME)  ||
         ( ptitime != NULL && I_TIME < 0 ) || ( ptftime != NULL && F_TIME <= 0 )  ) {
        fprintf(stderr, "ERROR: BAD USE OF INTERVAL TIME DIRECTIVE, INVALID INTERVAL! \n");
        exit(-1);
    }
    char smvBind[300];

    if(ptitime){
        void* vp[] = {ptitime};
        updateValue("I_TIME", vp, T_DIRECTIVE_ENTRY, 1, -1, -1, scope, 0);
        sprintf(smvBind,"%d",I_TIME);
        updateTime(controllerSmv->MAIN, controllerSmv->mainInfo, smvBind, T_DIRECTIVE_ENTRY, 0, *ptitime);
        // necessita atualizar C_TIME
        updateValue("C_TIME", vp, T_DIRECTIVE_ENTRY, 1, -1, -1, scope, 0);
    }
    if(ptftime){
        void* vp[] = {ptftime};
        updateValue("F_TIME", vp, T_DIRECTIVE_ENTRY, 1, -1, -1, scope, 0);
        sprintf(smvBind,"%d",F_TIME);
        updateTime(controllerSmv->MAIN, controllerSmv->mainInfo, smvBind, T_DIRECTIVE_ENTRY, 1, *ptftime);
    }
    return NULL;
}

/**
 * Método auxiliar para tratar atualização de valores de variáveis, incluindo de NULL e listas
 * @param varName o nome da variável
 * @param var o objeto referência da variável
 * @param expr o objeto referência da expressão (que será atirbuida a var)
 * @param scope o escopo atual
 * @param index o indice o qual a atribuição é feita caso seja uma lista
 * @param C_TIME a diretiva C_TIME
 * @SideEffects: Atualiza o valor de uma entrada na tabela de simbolos
 */
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

Object* eval_ASSIGN_TDIRECTIVE(Node* n, STable* scope, EnvController* controllerSmv){
    Object* expr = NULL;
    Object** sintExpr = NULL;
    // recuperação de diretiva temporal principal para uso
    TableEntry* ctimeEntry = lookup(scope, "C_TIME");
    int C_TIME = *(int*) ctimeEntry->val->values[0];
    // caso de atribuição de diretiva
    // verificação semântica (se pode atribuir)(essas atribuições só são válidas no programa principal) ? seria uma boa tratar na gramática (?)
    if(scope->type != GLOBAL)
    {
        fprintf(stderr, "ERROR: BAD USE OF %s TIME DIRECTIVE, THE CONTEXT IS LOCKED! \n", n->leafs[0]);
        exit(-1);
    }
    // dois casos: alterar o valor na tabela de simbolos
    //Mudou CONTEXTO (C_TIME) : seguir o caso default
    // objeto sintetizado (chamar fora ou dentro do if depois das validações? avisa erros mais rapido)
    expr = eval(n->children[0], scope, controllerSmv);
    if(expr && expr->type != NUMBER_ENTRY || expr->OBJECT_SIZE > 1)
    {
        fprintf(stderr, "ERROR: BAD USE OF %s TIME DIRECTIVE, ONLY NUMERICAL VALUES ARE ACCEPTED \n", n->leafs[0]);
        exit(-1);
    }
    if(*(int*)expr->values[0] < C_TIME ){
        fprintf(stderr, "ERROR: BAD USE OF %s TIME DIRECTIVE, IMPOSSIBLE TO RETURN TO PAST CONTEXTS  \n", n->leafs[0]);
        exit(-1);
    }
    // validaçao de intervalo
    TableEntry* itimeEntry = lookup(scope,"F_TIME");
    int ftime = *(int*)itimeEntry->val->values[0];
    if(*(int*)expr->values[0]  > ftime){
        fprintf(stderr, "WARNING: %s IS BEYOND THE OBSERVATION INTERVAL. The statement has no effect \n", n->leafs[0]);
    }
    else if(*(int*) expr->values[0] != C_TIME){
        commitCurrentTime(scope,controllerSmv,*(int*) expr->values[0]);
        void* vp[] = {expr->values[0]};
        updateValue(n->leafs[0], vp, T_DIRECTIVE_ENTRY, 1, -1, -1, scope, 0);
        letgoObject(expr);
    }
    return NULL;
}

Object* evalOTHER_ASSIGN(Node* n, STable* scope, EnvController* controllerSmv)
{
    Object* expr = NULL;
    Object** sintExpr = NULL;
    // busca expressão
    expr = eval(n->children[1], scope, controllerSmv);
    // busca a variável e seu contexto
    char* varName = n->children[0]->leafs[0];
    TableEntry* varEntry = lookup(scope,varName);
    Object* var = varEntry == NULL ?  NULL : varEntry->val;

    // ports ou main
    STable* refAuxTable = selectSMV_SCOPE(scope,controllerSmv);
    HeaderSmv* refHeader = selectSMV_INFO(scope,NULL,controllerSmv);

    // validação de tempo
    TableEntry* ctimeEntry = lookup(scope, "C_TIME");
    int C_TIME = *(int*) ctimeEntry->val->values[0];
    TableEntry* itimeEntry = lookup(scope,"I_TIME");
    int I_TIME = *(int*)itimeEntry->val->values[0];
    int changeContext = C_TIME > I_TIME; // verifica se mudou o contexto

    // atribuição simples
    if(expr && (expr->type != TDS_ENTRY || expr->aList ) && n->children[0]->type == ASSIGN_IDVAR)
    {
        //primeira vez da variavel (ou não inicializada, mudança para depois)
        if(!var)
        {
            if(!scope->notEvaluated){
                addValue(varName, expr->values, expr->type, expr->OBJECT_SIZE, 0, scope, C_TIME);
            }
            //inicialização "com next", necessita criar um default para os instantes anteriores e o seu next
            if(expr->type != NULL_ENTRY && !scope->notWrite){
                if(changeContext){
                    specAssign(1, varName, changeContext, refHeader, scope, refAuxTable, expr, 0, 0, C_TIME, controllerSmv);
                    specAssign(1, varName, changeContext, refHeader, scope, refAuxTable, expr, 0, 1, C_TIME, controllerSmv);
                }
                else{
                    specAssign(1, varName, changeContext, refHeader, scope, refAuxTable, expr, 0, 0, C_TIME, controllerSmv);
                }
            }
        }
        else{
            if(!scope->notEvaluated){
                updateVariable(varName,var,expr,scope,-1,C_TIME);
            }
            // tempo > 0 e não ocorreu redefinição
            if((expr->type != NULL_ENTRY && !scope->notWrite)){
                if(changeContext){
                    specAssign(0, varName, changeContext, refHeader, scope, refAuxTable, expr, var->redef, 1,
                                   C_TIME, controllerSmv);
                }
            }
        }
        letgoObject(expr);
    }
    else if(expr){
        if(var && expr->type != var->type){
            fprintf(stderr, "TYPE ERROR: %s datastructure type is imutable \n",varName);
            exit(-1); // por enquanto declarações unicas de estruturas de dados
        }
        if(!var)
        {
            addReferenceCurrentScope(varName,expr,0,scope);
            if(expr->type != TDS_ENTRY){
                // spec de lista
            }
        }
        else{
            // update ref
        }
    }
    return NULL;
}

Object * evalCMD_IF(Node* n, STable* scope, EnvController* controllerSmv){

    Object* conditionalExpr = NULL;

    STable* IF_SCOPE = addSubScope(scope,IF_BLOCK);
    conditionalExpr = eval(n->children[0], scope, controllerSmv);
    printObject(conditionalExpr);
    bindCondition(IF_SCOPE,conditionalExpr);
    IF_SCOPE->notEvaluated = !(!IF_SCOPE->parent->notEvaluated && *(int*)conditionalExpr->values[0]);
    eval(n->children[1], IF_SCOPE, controllerSmv);
    // cria else
    if(n->children[2]){
        STable* ELSE_SCOPE = addSubScope(scope,ELSE_BLOCK);
        ELSE_SCOPE->notEvaluated = !(!ELSE_SCOPE->parent->notEvaluated && !*(int*)conditionalExpr->values[0]);
        Object* notExpr = notObjectOperation(conditionalExpr);
        bindCondition(ELSE_SCOPE,notExpr);
        eval(n->children[2]->children[0], ELSE_SCOPE, controllerSmv);
        //letgoTable(ELSE_SCOPE);
    }
    else {
        letgoObject(conditionalExpr);
    }
    //  letgoTable(IF_SCOPE);
    return NULL;
}

void computeTDSDependentOperations(Node*n, char* portName, STable* scope, TDS* newTDS, EnvController* controller, int I_TIME, int C_TIME){
    Object * dependenceList = eval(n->children[0],scope,controller);
    Object * DEP_HEAD = dependenceList->OBJECT_SIZE > 1 ? dependenceList->values[0] : NULL;
    if(DEP_HEAD && DEP_HEAD->type == TDS_ENTRY || dependenceList->type == TDS_ENTRY) {
        if (dependenceList->OBJECT_SIZE > 2) {
            fprintf(stderr,
                    "[WARNING] %s definition uses more than 2 inputs.\nIt is recommended to check the input model, for instance, reo2nuXmv only generates merge-like connections with at most 2 inputs  \n",
                    portName);
        }
        if(( newTDS->delayed || dependenceList->OBJECT_SIZE > 1) && C_TIME > I_TIME){
            fprintf(stderr, "[WARNING] %s uses a temporal condition to initialization.\nIt is recommended to review your specification. This behaviour is usually acceptable in case the modeled port is lossy (eg: LossySync)\n",
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

    TDS* newTDS = createTDS(portName, type, tdsSpec, delayed,
                            type == FUNCTION_APPLY ? (char *) tdsSpec->values[0] : NULL, C_TIME, F_TIME, NULL);
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

Object * evalTDS_DEF_COMPLETE(Node* n, STable* scope, EnvController* controllerSmv){

    if(scope->type != GLOBAL){
        fprintf(stderr, "ERROR: BAD USE OF TDS DEFINITION, CONDITIONAL DEFINITIONS OF MODULES ARE NOT SUPPORTED BY nuXmv.\nPlease refer to the documentation for further info. \n");
    }
    char* portName = n->leafs[3];
    Object * domainInfo = eval(n->children[0],scope,controllerSmv);
    TDS_TYPE type =  domainInfo->type == TIME_COMPONENT? DATA_LIST :
                     domainInfo->type == FUNCTION_ENTRY? FUNCTION_APPLY : MATH_EXPRESSION;
    return computeTDSBasicOperations(NULL, portName, type, domainInfo, 0, scope, NULL, controllerSmv);
}

Object * evalANON_TDS(Node* n, STable* scope, EnvController* controllerSmv){
    int isDelayed = n->children[1]->type == TDS_ANON_OP_DPASS;
    int hasFilter = n->children[1]->type == TDS_ANON_OP_FPASS;
    Node* pathFilter = hasFilter ? n->children[1]->children[0] : NULL;
    Object* encapsulatedTDS = computeTDSBasicOperations(n, n->leafs[2], TDS_DEPEN, NULL,
                                                        n->children[1]->type == TDS_ANON_OP_DPASS, scope, pathFilter,
                                                        controllerSmv);
    TDS* newTDS = encapsulatedTDS->values[0];
    addReferenceCurrentScope(newTDS->name,encapsulatedTDS,0,scope);
    return encapsulatedTDS;
}


Object* evalTDS_DEF_DEPENDECE(Node* n, STable* scope, EnvController* controllerSmv){
    int hasOption = n->children[0]->nchild == 2;
    int isDelayed = hasOption && n->children[0]->children[1]->type == DEF_EXTRAS_DELAYED;
    Node* pathFilter = hasOption && !isDelayed ?
            n->children[0]->children[1]->children[0]
            : NULL;
    return computeTDSBasicOperations(n->children[0], n->leafs[3], TDS_DEPEN, NULL,
                                     isDelayed, scope, pathFilter, controllerSmv);
}

Object * evalTDS_DATA_TIME_COMPONENT(Node* n, STable* scope, EnvController* controllerSmv){

    //Object* (*SYNTH) (Node*, STable*, EnvController*) = eval;
    Node* PROGRAM_PATH = n->children[0];
/*    if(SYNTH->OBJECT_SIZE > 1){
        fprintf(stderr, "TDS data per time not compatible with lists. Please reffer to the documentation for further info. \n");
        exit(-1);
    }
    // VALIDAR NO PARSER? Ou na hora de sintetizar o que veio dessa time_component?
*/
    int* TIME = malloc(sizeof(int));
    *TIME = atoi(n->leafs[0]);
    void* TC[] = {TIME,PROGRAM_PATH};
    Object* SYNTH_TIME_COMPONENT = createObjectDS(TIME_COMPONENT,2,TC,-1,NULL,0);
    return SYNTH_TIME_COMPONENT;
}


Object * eval_ITERATOR(Node* n, STable* scope, EnvController* controllerSmv){

    if(n->nchild > 1){
        // deve retornar uma lista com ambos
        Object * LEFT_COMPONENT = eval(n->children[0],scope,controllerSmv); // vai ter sempre uma outra lista (que possui uma lista e 1 componente)
        // onde a lista possui uma lista e uma componente ...
        // essa lista tem tamanho: n
        Object * RIGHT_COMPONENT = eval(n->children[1],scope,controllerSmv);
        // ao terminar temos os membros da lista MAIS A ESQUERDA e MAIS A DIREITA

        Object* MERGED = mergeGenericList(LEFT_COMPONENT,RIGHT_COMPONENT);

        // juntando a lista com a componente ->  n + 1 (a chamada de cima vai sintetizar uma lista com n+1 membros e outra componnete)
        // mas como saber o tamanho quando ele subir o nível?
        // é util mantermos essa estrutura de Object**  ???  (Afinal, qualquer processamento que e feito em nos com mais de um filho
        // e por meio dos EXECUTORES)
        // unico caso útil seria na hora de sintetizar expressões complexas.

        // Note que voltar com Object* como era antes não resolve totalmente esse problema também, teriamos que ter um
        // "evalIterador" que constrói uma lista de objetos sintetizados.

        // A UNICA MANEIRA É USAR O PROPRIO OBJETCT e retornar só Object*
        // aproveitar da estrutura de lista dos objects para isso!
        // fazer um object que tem uma lista de objects do tipo TIME_COMPONENT (tempo, dado) -> util para evitar "problema de indices"
        // deve ter um novo tipo (ex: TIME_COMPONENT? ou TIME_COMPONENT_{tipoDado} ?) (ME PARECE MELHOR A PRIMEIRA)
        // e então dado É OUTRO OBJECT (que tem o seu tipo proprio)
        // problema: poderia ficar "redundante" e dificil de manipular considerando as listas.

        // note que essa estrutura deve ser construida em um método generico
        return MERGED;
    }
    else{
        Object * HEAD = eval(n->children[0],scope,controllerSmv);
        return HEAD;
    }
}

void startInterpreter(Node* n, STable* scope, EnvController* controller){
    /*
    * Realização de estrutura auxiliar
    */
    //Object ** REALIZATION = malloc(sizeof(Object*)*DEFAULT_MEMOI);
    //MEMOI = REALIZATION;
    eval(n,scope,controller);
    commitCurrentTime(scope,controller,*(int*) lookup(scope,"F_TIME")->val->values[0]);
    letgoNode(n);
}


