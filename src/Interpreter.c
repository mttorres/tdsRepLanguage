//
// Created by mateus on 28/10/2020.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <TDS.h>
#include "../headers/PostProcess.h"

typedef enum MAP_OP { PLUS = 43, MINUS = 45, TIMES = 42, DIVIDE = 47, MOD = 37, LT = 60, GT = 62, NOTEQUAL = 94, NOT_PREFIX = 110,
    LE = 121, EQUAL = 122, GE = 123} MAP_OP;

Object *eval(Node *n, STable *scope,  HeaderController *controllerSmv);
Object* evalNUM(Node* n, STable* scope,  HeaderController* controllerSmv);
Object* evalBOOL(Node* n, STable* scope,  HeaderController* controllerSmv);
Object* evalSTRING(Node* n, STable* scope,  HeaderController* controllerSmv);
Object* evalNULL(Node* n, STable* scope,  HeaderController* controllerSmv);
Object* evalIDVAR(Node* n, STable* scope,  HeaderController* controllerSmv);
Object* evalTIME_DIRECTIVE(Node* n, STable* scope,  HeaderController* controllerSmv);
Object* evalDataV(Node* n, STable* scope,  HeaderController* controllerSmv);
Object* evalPARAMS_CALL(Node* n, STable* scope,  HeaderController* controllerSmv);
Object* evalAC_V(Node* n, STable* scope,  HeaderController* controllerSmv);
Object* evalOTHER_ASSIGN(Node* n, STable* scope,  HeaderController* controllerSmv);
Object* evalV_PROP(Node* n, STable* scope,  HeaderController* controllerSmv);
Object* evalADD_V(Node* n, STable* scope,  HeaderController* controllerSmv);
Object* evalADD_V_PROP(Node* n, STable* scope,  HeaderController* controllerSmv);
Object* evalV_PROP_TDS(Node* n, STable* scope,  HeaderController* controllerSmv);
Object * evalEXPR(Node* n, STable* scope,  HeaderController* controllerSmv);
Object * evalDEFINE_INTERVAL(Node* n, STable* scope,  HeaderController* controllerSmv);
Object * evalCMD_IF(Node* n, STable* scope,  HeaderController* controllerSmv);
Object * evalTDS_DEF_COMPLETE(Node* n, STable* scope,  HeaderController* controllerSmv);
Object * evalTDS_DEF_DEPENDECE(Node* n, STable* scope,  HeaderController* controllerSmv);
Object * evalTDS_DATA_TIME_COMPONENT(Node* n, STable* scope,  HeaderController* controllerSmv);

Object* eval_ITERATOR(Node* n, STable* scope,  HeaderController* controllerSmv);

STable* selectSMV_SCOPE(STable* scope, HeaderController* controllerSmv){
    if(scope->type == FUNC || scope->childOfFunction){
        return accessSmvInfo(controllerSmv,FUNCTION_SMV,scope->indexRef);
//        return controllerSmv->functionsInfo[scope->indexRef];
    }
    else{
        return controllerSmv->mainInfo; // só vai retornar ports em casos de declarações de tds
    }
}

HeaderSmv * selectSMV_INFO(STable* scope, Object* functionPointer,HeaderController* controllerSmv){
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


void resolveTdsLazyEvaluation(STable *currentScope, HeaderController *controllerSmv, int C_TIME) {
    int I_TIME = *(int*) lookup(currentScope, "I_TIME")->val->values[0];
    Node* PROGRAM_PATH = NULL;
    Object* lazyValue = NULL;
    int i;
    for (i = 0; i < controllerSmv->declaredPortsNumber; i++) {
        // resolve call-by-need cada expressão ativa da TDS.
        TDS* currentTDS = controllerSmv->declaredPorts[i];
        if(currentTDS->type == DATA_LIST){
            // eval de forma que ele deve saber qual componente temporal ele deve pegar
            if(currentTDS->COMPONENT_TIMES[C_TIME] != -1){
                Object* timeComponent = (Object*) currentTDS->DATA_TIME->values[currentTDS->COMPONENT_TIMES[C_TIME]];
                PROGRAM_PATH = (Node*) timeComponent->values[1];
                lazyValue = eval(PROGRAM_PATH,currentScope,controllerSmv);
                specTDS(currentTDS,lazyValue,C_TIME,I_TIME,controllerSmv,currentScope);
            }
        }else{
            // eval da forma "valor unico" (é uma operação call-by need em contexto de escopo)
            lazyValue = NULL;
            PROGRAM_PATH = NULL; // por enquanto
            specTDS(currentTDS,lazyValue,C_TIME,I_TIME,controllerSmv,currentScope);
        }

    }
}

// CASO FORA DE FLUXO 1: e se ele "pular", ex: commitar c_time = 2 (quando era 0 antes), ele pulou o 1! A gente deve ver a "diferença"
// CHAMAR ESSA FUNÇÃO DIFERENÇA VEZES.
// CASO FORA DE FLUXO 2: deve chamar isso também ao FIM DO PROGRAMA (para cada CHAMADA RESTANTE, ex: C_TIME terminou em 4, deve chamar até F_TIME (5, ... , F_TIME).
// DIFERENÇA VEZES DE NOVO!
void commitCurrentTime(STable* currentScope, HeaderController* controllerSmv, int changedTo){
    // deve resolver a avaliação para cada TDS "n" vezes. Antes do proximo intervalo ou fim do programa.
    int i;
    int C_TIME = *(int*) lookup(currentScope,"C_TIME")->val->values[0];
    for (i = C_TIME; i < changedTo; i++) {
        resolveTdsLazyEvaluation(currentScope, controllerSmv, i);
    }
}

Object* (*executores[80]) (Node* n, STable* scope,  HeaderController* controllerSmv) = {

        evalNUM, evalBOOL, evalSTRING, evalNULL, evalIDVAR, evalTIME_DIRECTIVE, evalDataV, evalPARAMS_CALL, evalDEFINE_INTERVAL ,evalAC_V,
        evalOTHER_ASSIGN, evalV_PROP, evalADD_V, evalADD_V_PROP, evalV_PROP_TDS, evalEXPR, evalCMD_IF,
        evalTDS_DEF_COMPLETE, evalTDS_DEF_DEPENDECE, eval_ITERATOR, evalTDS_DATA_TIME_COMPONENT
};


Object* evalNUM(Node* n, STable* scope,  HeaderController* controllerSmv)
{
    int sint;
    sint = atoi(n->leafs[0]);
    printf("[evalNUM] SINTH: %d \n",sint);
    void* ip[] = {&sint};
    Object* o = createObject(NUMBER_ENTRY, 1, ip, -1, n->leafs[0]);
    return o;
}

Object* evalBOOL(Node* n, STable* scope,  HeaderController* controllerSmv)
{
    printf("[evalBOOL] \n");
    int sint;

    char* trueString = "true";

    sint= strstr(n->leafs[0],trueString)? 1 : 0;

    printf("[evalBOOL] SINTH: %d \n",sint);

    void* bp[] = {&sint};

    Object* o = createObject(LOGICAL_ENTRY, 1, bp, -1, NULL);

    return o;
}


Object* evalSTRING(Node* n, STable* scope,  HeaderController* controllerSmv)
{
    printf("[evalSTRING] \n");
    char* sint =  n->leafs[0];

    void* sp[] = {sint};

    printf("[evalSTRING] SINTH: %s \n",sint);

    Object* o = createObject(LABEL_ENTRY, 1, sp, -1, NULL);

    return o;
}


/*
	Pensar depois... encapsular NULL ?

*/

Object* evalNULL(Node* n, STable* scope,  HeaderController* controllerSmv)
{
    printf("[evalNULL] \n");
    // se eu interpretar como "NULL" do C mesmo podemos ter problemas(?)
    char* sint =  n->leafs[0];

    void* np[] = {sint};

    printf("[evalNULL] SINTH: %s \n",sint);

    Object* o = createObject(NULL_ENTRY, 1, np, -1, NULL);

    return o;
}


Object* evalIDVAR(Node* n, STable* scope,  HeaderController* controllerSmv)
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
        if(entry->val->type == TDS_ENTRY || entry->val->OBJECT_SIZE > 1)
        {
            return entry->val;
        }
        else
        {
            // copia o objeto atomico (TEM QUE PASSAR O BIND NOVO!)
            return refCopyOfVariable(entry);
            //return copyObject(entry->val);
        }
    }
}


Object* evalTIME_DIRECTIVE(Node* n, STable* scope,  HeaderController* controllerSmv)
{
    printf("[evalTIME_DIRECTIVE] \n");

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
        return createObject(NUMBER_ENTRY, 1, entry->val->values, -1, NULL);
    }
}



Object* evalDataV(Node* n, STable* scope,  HeaderController* controllerSmv)
{
    printf("[evalDataV] \n");


    //str= evalSTRING(n,  scope, writeSmvTypeTable, controllerSmv);

    //printf("[evalDataV] DATA_V SINTH: %s \n",str);

    //return sint;

    // esse vai ser bem diferente...

    // vai chamar evalParams , e sintetizar um Object Vetor (ou um vetor void que será jogado em um object)
}

Object* evalPARAMS_CALL(Node* n, STable* scope,  HeaderController* controllerSmv)
{
    printf("[evalPARAMS_CALL] \n");
}




Object* evalPLUS(Object* o1, Object* o2)
{
    printf("[evalPLUS] \n");
    int* r;
    void* rp[1];
    if(o1->type == NUMBER_ENTRY)
    {
        *r = (*(int*)o1->values[0]) + (*(int*)o2->values[0]);
        rp[0] = r;
        return createObject(NUMBER_ENTRY, 1, rp, -1, NULL);
    }
    if(o1->type == LABEL_ENTRY)
    {
        // concatena

    }

}

Object* evalMINUS(Object* o1, Object* o2)
{
    if(o1->type != NUMBER_ENTRY || (o2 && o2->type != NUMBER_ENTRY))
    {
        fprintf(stderr, "INCOMPATIBLE OPERANDS FOR THE (-) OPERATION!");
        exit(-1);
    }
    printf("[evalMINUS] \n");
    int r;
    r =  o2 == NULL? (-1)*(*(int*) o1->values[0]) : (*(int*)o1->values[0]) - (*(int*)o2->values[0]);
    void* rp[] = {&r};
    letgoObject(o1);
    if(o2){
        letgoObject(o2);
    }
    return createObject(NUMBER_ENTRY, 1, rp, -1, NULL);
}

Object* evalNOT(Object* o)
{
    if(o->type != LOGICAL_ENTRY)
    {
        fprintf(stderr, "INCOMPATIBLE OPERANDS FOR THE (not) OPERATION!");
        exit(-1);
    }
    printf("[evalNOT] \n");
    int r;
    r =  !(*(int*) o->values[0]);
    void* rp[] = {&r};
    char resultingBind[strlen(o->SINTH_BIND)+2];
    createExprBind(resultingBind, o, NULL, "!");
    letgoObject(o);
    return createObject(LOGICAL_ENTRY, 1, rp, -1, resultingBind);
}

Object* evalTIMES(Object* o1, Object* o2)
{
    int* r;
    *r = (*(int*)o1->values[0]) * (*(int*)o2->values[0]);
    void* rp[] = {r};
    return createObject(NUMBER_ENTRY, 1, rp, -1, NULL);
}

Object* evalDIVIDE(Object* o1, Object* o2, int type)
{
    printf("[evalDIVIDE] \n");
    void* rp[1];
    if(*(int*)o2->values[0] == 0)
    {
        fprintf(stderr, "CANNOT DIVIDE BY ZERO!");
        exit(-1);
    }
    int* r;
    *r = type == DIVIDE? (*(int*)o1->values[0]) / (*(int*)o2->values[0]) : (*(int*)o1->values[0]) % (*(int*)o2->values[0]);
    rp[0] = r;

    return createObject(NUMBER_ENTRY, 1, rp, -1, NULL);
}

Object* evalLESS(Object* o1, Object* o2, int opCode)
{
    printf("[evalLESS] \n");
    void* rp[1];
    int* r;
    if(o1->OBJECT_SIZE > 1 || o2->OBJECT_SIZE > 1)
    {
        *r = opCode == LT? o1->OBJECT_SIZE < o2->OBJECT_SIZE : o1->OBJECT_SIZE <= o2->OBJECT_SIZE;
    }
    else
    {
        if(o1->type == NUMBER_ENTRY)
        {
            *r = opCode == LT? (*(int*)o1->values[0]) < (*(int*)o2->values[0]) : (*(int*)o1->values[0]) <= (*(int*)o2->values[0]);
        }
        if(o1->type == LABEL_ENTRY)
        {
            //teste o tamanho salvo no object 1 e object 2
        }
    }
    rp[0] = r;
    return createObject(LOGICAL_ENTRY, 1, rp, -1, NULL);
}

Object* evalGREAT(Object* o1, Object* o2, int opCode)
{
    printf("[evalGREAT] \n");
    void* rp[1];
    int* r;
    if(o1->OBJECT_SIZE > 1 ||o2->OBJECT_SIZE > 1)
    {
        *r = opCode == GT? o1->OBJECT_SIZE > o2->OBJECT_SIZE : o1->OBJECT_SIZE >= o2->OBJECT_SIZE;
    }
    else
    {
        if(o1->type == NUMBER_ENTRY)
        {
            *r = opCode == GT? (*(int*)o1->values[0]) > (*(int*)o2->values[0]) : (*(int*)o1->values[0]) >= (*(int*)o2->values[0]);
        }
        if(o1->type == LABEL_ENTRY)
        {
            //teste o tamanho salvo no object 1 e object 2
        }
    }
    rp[0] = r;
    return createObject(LOGICAL_ENTRY, 1, rp, -1, NULL);

}

Object* evalEqual(Object* o1, Object* o2, int opCode)
{
    printf("[evalEqual] \n");
    void* rp[1];
    int* r;
    if(o1->OBJECT_SIZE > 1 ||o2->OBJECT_SIZE > 1)
    {
        *r = opCode == EQUAL? o1->OBJECT_SIZE == o2->OBJECT_SIZE : o1->OBJECT_SIZE != o2->OBJECT_SIZE;
    }
    else
    {
        if(o1->type == NUMBER_ENTRY)
        {
            *r = opCode == EQUAL? (*(int*)o1->values[0]) == (*(int*)o2->values[0]) : (*(int*)o1->values[0]) != (*(int*)o2->values[0]);
        }
        if(o1->type == LABEL_ENTRY)
        {
            //teste o tamanho salvo no object 1 e object 2
        }
    }
    rp[0] = r;
}



Object* evalEXPR(Node* n, STable* scope,  HeaderController* controllerSmv)
{
    printf("[evalEXPR] \n");
    // operação unária ou simplesmente FOLHA
    if(n->nchild <= 1)
    {
        Object * sintUni = eval(n->children[0], scope, controllerSmv);

        char ops[1];
        // caso de operação unária (tem folhas e filhos)
        if(n->nchild == 1 && n->nleafs == 1 )
        {
            ops[0] = n->leafs[0][0]; // recupera a operação

            if(ops[0] == MINUS  && sintUni->type == NUMBER_ENTRY)
            {
                return evalMINUS(sintUni,NULL);
            }
            if(ops[0] == NOT_PREFIX  && sintUni->type == LOGICAL_ENTRY)
            {
                return evalNOT(sintUni);
            }
            fprintf(stderr, "INCOMPATIBLE OPERANDS FOR THE %c OPERATION!", ops[0]);
            exit(-1);
        }
        return sintUni;
    }
    // operação binária
    else
    {
        // CUIDADO (ordem avaliação)
        Object* o1 = evalEXPR(n->children[0], scope, controllerSmv);
        Object* o2 = evalEXPR(n->children[1],scope, controllerSmv);

        int sOp = strlen(n->leafs[0]);
        char ops[2];
        ops[1] = '\0';

        if(sOp > 1)
        {
            ops[1] = n->leafs[0][1];
        }
        ops[0] = n->leafs[0][0];


        // validações (adicionar uma para vetores depois (não deixar fazer nenhuma operação com eles exceto comparação de tamanho))
        if(o1->type == TDS_ENTRY || o2->type == TDS_ENTRY)
        {
            if(sOp < 1)
            {
                fprintf(stderr, "TDS INCOMPATIBLE FOR THE %c OPERATION! ", ops[0]);
                exit(-1);
            }
            else
            {
                fprintf(stderr, "TDS INCOMPATIBLE FOR THE %c%c OPERATION!", ops[0],ops[1]);
                exit(-1);
            }
        }

        if(o1->type == NULL_ENTRY || o2->type == TDS_ENTRY)
        {
            fprintf(stderr, "NULL REFERENCE FOR THE %c%c OPERATION! ", ops[0],ops[1]);
            exit(-1);
        }

        if(o1->type != o2->type)
        {
            if(sOp < 1)
            {
                fprintf(stderr, "INCOMPATIBLE TYPES FOR THE %c OPERATION!", ops[0]);
                exit(-1);
            }
            else
            {
                fprintf(stderr, "INCOMPATIBLE TYPES FOR THE %c%c OPERATION!", ops[0],ops[1]);
                exit(-1);
            }
        }

        int vetOp = o1->OBJECT_SIZE > 1 ||o2->OBJECT_SIZE > 1;

        if(!vetOp && ops[0] == PLUS)
        {
            return evalPLUS(o1,o2);
        }
        if(!vetOp && ops[0] == MINUS && o1->type != LABEL_ENTRY)
        {
            return evalMINUS(o1,o2);
        }
        if(!vetOp && ops[0] == TIMES && o1->type != LABEL_ENTRY)
        {
            return evalTIMES(o1,o2);
        }
        if(!vetOp && ops[0] == DIVIDE && o1->type != LABEL_ENTRY)
        {
            return evalDIVIDE(o1,o2,DIVIDE);
        }
        if(!vetOp && ops[0] == MOD && o1->type != LABEL_ENTRY)
        {
            return evalDIVIDE(o1,o2,MOD);
        }
        if(ops[0] == LT)
        {
            if(sOp > 1)
            {
                return evalLESS(o1,o2,LE);
            }
            else
            {
                evalLESS(o1,o2,LT);
            }
        }
        if(ops[0] == GT)
        {
            if(sOp > 1)
            {
                return evalGREAT(o1,o2,GE);
            }
            return evalGREAT(o1,o2,GT);
        }
        else
        {
            return(evalEqual(o1,o2,ops[0]+ops[2]));
        }

        fprintf(stderr, "INCOMPATIBLE OPERANDS FOR THE %c%c OPERATION!", ops[0],ops[1]);
        exit(-1);
    }

}


Object* evalProp(Node* fatherRef, Node* n, STable* scope,  HeaderController* controllerSmv)
{
    printf("[evalProp] \n");
    if(!n)
    {
        printf("[evalProp] VARIAVEL: \n");
        // NÃO PRECISA RETORNAR NADA, já temos a referência no nó acima
        // até porque não temos nada
        return NULL;
    }
    if(n->type == V_PROP)
    {

        char* propName;

        printf("[evalProp] PROP VARIAVEL: %s \n",propName);
        //return createObject();
        return NULL;
    }
    if(n->type == ADD_V)
    {
        Object* expr = evalEXPR(n->children[0],scope, controllerSmv);
        if(expr->type != NUMBER_ENTRY)
        {
            fprintf(stderr, "%s: INVALID INDEX!", fatherRef->leafs[0]);
            exit(-1);
        }
        printf("[evalProp] VARIAVEL pos: %d \n",expr->values[0]);
        return expr;
    }

    char* propName;
    printf("[evalProp] VARIAVEL pos prop %d \n",propName);
    return NULL;

}


Object* evalAC_V(Node* n, STable* scope,  HeaderController* controllerSmv)
{
    printf("[evalAC_V] \n");

    // recupera a variável e o nome do atributo dela logo após
    TableEntry* entry = lookup(scope,n->leafs[0]);

    if(!entry)
    {
        fprintf(stderr, "%s NOT DECLARED!", n->leafs[0]);
        exit(-1);
    }
    else
    {
        Object* prop = evalProp(n,n->children[0], scope, controllerSmv);

        if(entry->val->OBJECT_SIZE > 1)
        {
            // retorna a referência (ai pode sim ter colaterais)
            return entry->val;
        }
        else
        {
            // copia o objeto atomico
            return refCopyOfVariable(entry);
        }
    }
}

Object* evalV_PROP(Node* n, STable* scope,  HeaderController* controllerSmv){
    printf("[evalV_PROP] \n");

}
Object* evalADD_V(Node* n, STable* scope,  HeaderController* controllerSmv){
    printf("[evalADD_V] \n");
}
Object* evalADD_V_PROP(Node* n, STable* scope,  HeaderController* controllerSmv){
    printf("[evalADD_V_PROP] \n");
}

Object* evalV_PROP_TDS(Node* n, STable* scope,  HeaderController* controllerSmv){
    printf("[evalV_PROP_TDS] \n");
}

Object * evalDEFINE_INTERVAL(Node* n, STable* scope,  HeaderController* controllerSmv){

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
        updateTime(controllerSmv->MAIN,controllerSmv->mainInfo,smvBind,NUMBER_ENTRY,0,0);
        // necessita atualizar C_TIME
        updateValue("C_TIME", vp, T_DIRECTIVE_ENTRY, 1, -1, -1, scope, 0);
    }
    if(ptftime){
        void* vp[] = {ptftime};
        updateValue("F_TIME", vp, T_DIRECTIVE_ENTRY, 1, -1, -1, scope, 0);
        sprintf(smvBind,"%d",F_TIME);
        updateTime(controllerSmv->MAIN,controllerSmv->mainInfo,smvBind,NUMBER_ENTRY,1,1);
    }
    return NULL;
}

Object* evalOTHER_ASSIGN(Node* n, STable* scope,  HeaderController* controllerSmv)
{
    Object* expr = NULL;
    Object** sintExpr = NULL;
    // recuperação de diretiva temporal principal para uso
    TableEntry* ctimeEntry = lookup(scope, "C_TIME");
    int C_TIME = *(int*) ctimeEntry->val->values[0];
    // caso de atribuição de diretiva
    if(n->children[0]->type == ASSIGN_TDIRECTIVE)
    {
        // verificação semântica (se pode atribuir)(essas atribuições só são válidas no programa principal) ? seria uma boa tratar na gramática (?)
        if(scope->type != GLOBAL)
        {
            fprintf(stderr, "ERROR: BAD USE OF %s TIME DIRECTIVE, THE CONTEXT IS LOCKED! \n", n->children[0]->leafs[0]);
            exit(-1);
        }
        // dois casos: alterar o valor na tabela de simbolos
        //Mudou intervalos: alterar intervalo no main do SMV e mudar o init ou next (dependendo da diretiva)
        //Mudou CONTEXTO (C_TIME) : seguir o caso default
        // objeto sintetizado (chamar fora ou dentro do if depois das validações? avisa erros mais rapido)
        expr = eval(n->children[1], scope, controllerSmv);
        if(expr && expr->type != NUMBER_ENTRY && expr->OBJECT_SIZE > 1)
        {
            fprintf(stderr, "ERROR: BAD USE OF %s TIME DIRECTIVE, ONLY NUMERICAL VALUES ARE ACCEPTED \n", n->children[0]->leafs[0]);
            exit(-1);
        }
        if(*(int*)expr->values[0] < C_TIME ){
            fprintf(stderr, "ERROR: BAD USE OF %s TIME DIRECTIVE, IMPOSSIBLE TO RETURN TO PAST CONTEXTS  \n", n->children[0]->leafs[0]);
            exit(-1);
        }
        // validaçao de intervalo
        TableEntry* itimeEntry = lookup(scope,"F_TIME");
        int ftime = *(int*)itimeEntry->val->values[0];
        if(C_TIME > ftime){
            fprintf(stderr, "WARNING: %s IS BEYOND THE OBSERVATION INTERVAL \n", n->children[0]->leafs[0]);
        }
        /* TOMAR NOTA: NUNCA MAIS FAZER ISSO
         * 		int* v;
         *       v = 5;  (PODE LITERALMENTE ALOCAR QUALQUER, REPITO QUALQUER REGIÃO DE MEMÓRIA PARA MEU 5!
         *       void* vp[] = {v};
         * */
        // só fazer isso se eu tiver dado malloc em v!
        // antes de realizar a mudança, devemos dar commit da TDS! (Nada atualmente impede o usuario de commitar "duas vezes", isso e um fail safe)
        if(*(int*) expr->values[0] != C_TIME){
            commitCurrentTime(scope,controllerSmv,*(int*) expr->values[0]);
        }
        void* vp[] = {expr->values[0]};
        updateValue(n->children[0]->leafs[0], vp, T_DIRECTIVE_ENTRY, 1, -1, -1, scope, 0);
        letgoObject(expr);
    }
    else{
        // busca expressão
        expr = eval(n->children[1], scope, controllerSmv);

        // busca a variável e seu contexto
        char* varName = n->children[0]->leafs[0];
        TableEntry* varEntry = lookup(scope,varName);
        Object* var = varEntry == NULL ?  NULL : varEntry->val;

        STable* refAuxTable = selectSMV_SCOPE(scope,controllerSmv);
        // ports ou main
        HeaderSmv* refHeader = selectSMV_INFO(scope,NULL,controllerSmv);

        TableEntry* itimeEntry = lookup(scope,"I_TIME");
        int I_TIME = *(int*)itimeEntry->val->values[0];
        int changeContext = C_TIME > I_TIME; // verifica se mudou o contexto

        // atribuição simples
        if(expr && expr->type != TDS_ENTRY && n->children[0]->type == ASSIGN_IDVAR)
        {
            //primeira vez da variavel (ou não inicializada, mudança para depois
            if(!var)
            {
                //int isTDS = expr->type == TDS_ENTRY;
                if(!scope->notEvaluated){
                    addValue(varName, expr->values, expr->type, expr->OBJECT_SIZE, 0, scope, C_TIME);
                }
                //inicialização "com next", necessita criar um default para os instantes anteriores e o seu next
                // note que temporal condition tem que ser um cubo de condição e tempo
                if(changeContext){
                    specAssign(1, varName, changeContext, refHeader, scope, refAuxTable, expr, 0, 0, C_TIME);
                    specAssign(1, varName, changeContext, refHeader, scope, refAuxTable, expr, 0, 1, C_TIME);
                }
                else{
                    specAssign(1, varName, changeContext, refHeader, scope, refAuxTable, expr, 0, 0, C_TIME);
                }
            }
            else{
                // VALIDAÇÃO DE TIPOS
                if(var->type != expr->type){
                    fprintf(stderr, "ASSIGN ERROR: incompatible type for  %s \n", varEntry->name);
                    exit(-1);
                }
                int prevDef = var->redef;
                int prevContext = var->timeContext;
                if(!scope->notEvaluated){
                    updateValue(varName, expr->values, expr->type, expr->OBJECT_SIZE, -1, -1, scope, C_TIME);
                }
                // tempo > 0 e não ocorreu redefinição
                if(changeContext && var->redef == prevDef){
                    specAssign(0, varName, changeContext, refHeader, scope, refAuxTable, expr, var->redef, 1, C_TIME);
                }
                else{
                    fprintf(stderr, "ASSIGN ERROR: redefinition of %s in same time interval \n", varEntry->name);
                    exit(-1);
                    /* casos de redefinição (devemos dar free na entrada anterior (otimização)
                    letGoOldEntry(varEntry,refAuxTable);
                    // tempo = 0, redefinição
                    if(!changeContext){
                        specAssign(0, varEntry, changeContext, refHeader, scope, refAuxTable, expr, var->redef, 0,
                                   C_TIME);
                    }
                    // tempo > 0 e redefinição
                    else{
                        specAssign(0, varEntry, changeContext, refHeader, scope, refAuxTable, expr, var->redef, 0,
                                   C_TIME);
                        specAssign(0, varEntry, changeContext, refHeader, scope, refAuxTable, expr, var->redef, 1,
                                   C_TIME);
                    }
                     */
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
                if(expr->type == TDS_ENTRY){
                    int F_TIME = *(int*)lookup(scope,"F_TIME")->val->values[0];
                    preProcessTDS(expr,controllerSmv,C_TIME,I_TIME,F_TIME);
                }
                addReferenceCurrentScope(varName,expr,0,scope);
            }

/*            Node* ref = n->children[0]->children[0];
            Object* indexRef = ref->type == V_PROP ? NULL : eval(n, scope, controllerSmv);

            if(ref->type == V_PROP || ref->type == ADD_V_PROP)
            {
                if(var->type != TDS_ENTRY)
                {
                    // TODO:  FUNÇÃO AVALIADORA DE TDS_PROP
                    fprintf(stderr, "ERROR: %s does not contain the %s property \n",varName,ref->children[0]->leafs[0]);
                    exit(-1);
                }

                else{
                    // retorna a prop...
                }
            }
            else{
                if(indexRef->type != NUMBER_ENTRY || indexRef->OBJECT_SIZE > 1)
                {
                    fprintf(stderr, "ERROR: %s cannot be indexed by non numerical values! \n",varName);
                    exit(-1);
                }
                int index = *(int*) indexRef->values[0];
                if(index >= var->OBJECT_SIZE){
                    fprintf(stderr, "ERROR: %s[%d] is out of bounds! \n",varName,index);
                    exit(-1);
                }
                // ainda parcialmente incompleto
                updateValue(varName, expr->values, var->type, var->OBJECT_SIZE, index, -1, scope, 0);
            }
                    */
        }

    }
    return NULL;
}

Object * evalCMD_IF(Node* n, STable* scope,  HeaderController* controllerSmv){

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
        Object* notExpr =  evalNOT(conditionalExpr);
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


Object * evalTDS_DEF_COMPLETE(Node* n, STable* scope,  HeaderController* controllerSmv){

    if(scope->type != GLOBAL){
        fprintf(stderr, "ERROR: BAD USE OF TDS DEFINITION, CONDITIONAL DEFINITIONS OF MODULES ARE NOT SUPPORTED BY nuXmv. "
                        "Please refer to the documentation for further info. \n");
    }
    char* portName = n->leafs[3];
    Object * domainInfo = eval(n->children[0],scope,controllerSmv);
    TDS_TYPE type = domainInfo->type == GEN_LIST ? DATA_LIST : domainInfo->type == FUNCTION_ENTRY? FUNCTION_APPLY : MATH_EXPRESSION;

    int C_TIME = *(int*) lookup(scope,"C_TIME")->val->values[0];
    int F_TIME  = *(int*) lookup(scope,"F_TIME")->val->values[0];

    TDS* newTDS = createTDS(portName, type, domainInfo, NULL, 0,
                            type == FUNCTION_APPLY ? (char *) domainInfo->values[0] : NULL, C_TIME, F_TIME);

    void* vp[] = {newTDS};
    char* TDS_BIND = createReferenceTDS(portName);
    Object* encapsulatedTDS = createObject(TDS_ENTRY,1,vp,-1,TDS_BIND);
    free(TDS_BIND); // pode parecer "irrelevante" mas é uma garantia, o createObject não cuida do free dos binds. Em especial por causa da cópia de variáveis.
    return encapsulatedTDS;
}

Object* evalTDS_DEF_DEPENDECE(Node* n, STable* scope,  HeaderController* controllerSmv){
    return NULL;
}

Object * evalTDS_DATA_TIME_COMPONENT(Node* n, STable* scope,  HeaderController* controllerSmv){

    //Object* (*SYNTH) (Node*, STable*, HeaderController*) = eval;
    Node* PROGRAM_PATH = n->children[0];
/*    if(SYNTH->OBJECT_SIZE > 1){
        fprintf(stderr, "TDS data per time not compatible with lists. Please reffer to the documentation for further info. \n");
        exit(-1);
    }
    // VALIDAR NO PARSER? Ou na hora de sintetizar o que vei odessa time_component?
*/
    int TIME = atoi(n->leafs[0]);
    void* TC[] = {&TIME,PROGRAM_PATH};
    Object* SYNTH_TIME_COMPONENT = createObject(TIME_COMPONENT,2,TC,-1,NULL);
    return SYNTH_TIME_COMPONENT;
}


Object * eval_ITERATOR(Node* n, STable* scope,  HeaderController* controllerSmv){

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

    return NULL;
}

Object *eval(Node *n, STable *scope, HeaderController *controllerSmv)
{
    //printf("[eval] %s \n",n->name);
    if(n)
    {
        // sintetizado dos filhos
        //void** SYNTH_C[n->nchild];
        // sintetizado dos filhos
        //void** SYNTH_L[n->nleafs];
        Object* SYNTH_O = NULL;

        if(executores[n->type])
        {
            printf("[PostProcess - eval] eval especifico \n\n");
            // o nó pode ou não ter mais dem filho, e esse trata o processamento desses nao sendo somente uma varredura e largura.
            SYNTH_O = executores[n->type](n,scope,controllerSmv);
        }
        else
        {
            //printf("[PostProcess - eval] eval genérico \n");
            if(n->nchild && n->children)
            {
                int i; // passa a diante (varredura em pos ordem)
                for (i=0; i < n->nchild; i++){
                    eval(n->children[i],scope,controllerSmv);
                }
            }
            if(n->type == PROG){
                // terminou
                int F_TIME = *(int*) lookup(scope,"F_TIME")->val->values[0];
                commitCurrentTime(scope,controllerSmv,F_TIME);
            }
        }
        return SYNTH_O;
    }
    return NULL;
}


