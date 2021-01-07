//
// Created by mateus on 28/10/2020.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../headers/PostProcess.h"
#include "../headers/InterpreterEvals.h"

typedef enum MAP_OP { PLUS = 43, MINUS = 45, TIMES = 42, DIVIDE = 47, MOD = 37, LT = 60, GT = 62, NOTEQUAL = 94, NOT_PREFIX = 110,
    LE = 121, EQUAL = 122, GE = 123} MAP_OP;

STable* selectSMV_SCOPE(STable* scope, HeaderController* controllerSmv){
    if(scope->type == FUNC || scope->childOfFunction){
        return controllerSmv->functionsInfo[scope->order];
    }
    else{
        return controllerSmv->mainInfo; // só vai retornar ports em casos de declarações de tds
    }
}

HeaderSmv * selectSMV_INFO(STable* scope, Object* functionPointer,HeaderController* controllerSmv){
    if(!functionPointer){
        return controllerSmv->MAIN_RELATED[0];
    }
    else{
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

Object* (*executores[80]) (Node* n, STable* scope, HeaderController* controllerSmv) = {

        evalNUM, evalBOOL, evalSTRING, evalNULL, evalIDVAR, evalTIME_DIRECTIVE, evalDataV, evalPARAMS_CALL, evalDEFINE_INTERVAL ,evalAC_V,
        evalOTHER_ASSIGN, evalV_PROP, evalADD_V, evalADD_V_PROP, evalV_PROP_TDS, evalEXPR, evalCMD_IF, evalMATCH_IF
};


Object* evalNUM(Node* n, STable* scope, HeaderController* controllerSmv)
{
    int sint;
    sint = atoi(n->leafs[0]);
    printf("[evalNUM] SINTH: %d \n",sint);
    void* ip[] = {&sint};
    Object* o = createObject(NUMBER_ENTRY, 1, ip, -1, NULL);
    return o;
}

Object* evalBOOL(Node* n, STable* scope, HeaderController* controllerSmv)
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


Object* evalSTRING(Node* n, STable* scope, HeaderController* controllerSmv)
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

Object* evalNULL(Node* n, STable* scope, HeaderController* controllerSmv)
{
    printf("[evalNULL] \n");
    // se eu interpretar como "NULL" do C mesmo podemos ter problemas(?)
    char* sint =  n->leafs[0];

    void* np[] = {sint};

    printf("[evalNULL] SINTH: %s \n",sint);

    Object* o = createObject(NULL_ENTRY, 1, np, -1, NULL);

    return o;
}


Object* evalIDVAR(Node* n, STable* scope, HeaderController* controllerSmv)
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

        if(entry->val->OBJECT_SIZE > 1)
        {
            // retorna a referência (ai pode sim ter colaterais)
            return entry->val;
        }
        else
        {
            // copia o objeto atomico
            return copyObject(entry->val);

        }
    }
}


Object* evalTIME_DIRECTIVE(Node* n, STable* scope, HeaderController* controllerSmv)
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



Object* evalDataV(Node* n, STable* scope, HeaderController* controllerSmv)
{
    printf("[evalDataV] \n");


    //str= evalSTRING(n,  scope, writeSmvTypeTable, controllerSmv);

    //printf("[evalDataV] DATA_V SINTH: %s \n",str);

    //return sint;

    // esse vai ser bem diferente...

    // vai chamar evalParams , e sintetizar um Object Vetor (ou um vetor void que será jogado em um object)
}

Object* evalPARAMS_CALL(Node* n, STable* scope, HeaderController* controllerSmv)
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



Object* evalEXPR(Node* n, STable* scope, HeaderController* controllerSmv)
{
    printf("[evalEXPR] \n");
    // operação unária ou simplesmente FOLHA
    if(n->nchild <= 1)
    {
        Object * sintUni;
        Object** sint = eval(n->children[0], scope, controllerSmv);
        sintUni = sint[0];

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
        free(sint);
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


Object* evalProp(Node* fatherRef, Node* n, STable* scope, HeaderController* controllerSmv)
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


Object* evalAC_V(Node* n, STable* scope, HeaderController* controllerSmv)
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
            return copyObject(entry->val);

        }
    }
}

Object* evalV_PROP(Node* n, STable* scope, HeaderController* controllerSmv){
    printf("[evalV_PROP] \n");

}
Object* evalADD_V(Node* n, STable* scope, HeaderController* controllerSmv){
    printf("[evalADD_V] \n");
}
Object* evalADD_V_PROP(Node* n, STable* scope, HeaderController* controllerSmv){
    printf("[evalADD_V_PROP] \n");
}

Object* evalV_PROP_TDS(Node* n, STable* scope, HeaderController* controllerSmv){
    printf("[evalV_PROP_TDS] \n");
}

Object * evalDEFINE_INTERVAL(Node* n, STable* scope, HeaderController* controllerSmv){

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
        updateTime(controllerSmv->MAIN_RELATED[0],controllerSmv->mainInfo,smvBind,NUMBER_ENTRY,0,ptitime);
        // necessita atualizar C_TIME
        updateValue("C_TIME", vp, T_DIRECTIVE_ENTRY, 1, -1, -1, scope, 0);
    }
    if(ptftime){
        void* vp[] = {ptftime};
        updateValue("F_TIME", vp, T_DIRECTIVE_ENTRY, 1, -1, -1, scope, 0);
        sprintf(smvBind,"%d",F_TIME);
        updateTime(controllerSmv->MAIN_RELATED[0],controllerSmv->mainInfo,smvBind,NUMBER_ENTRY,1,ptftime);
    }
    return NULL;
}

Object* evalOTHER_ASSIGN(Node* n, STable* scope, HeaderController* controllerSmv)
{
    Object* expr = NULL;
    Object** sintExpr = NULL;
    TableEntry* ctimeEntry = lookup(scope, "C_TIME");
    int ctime = *(int*) ctimeEntry->val->values[0];     // recuperação de diretiva temporal principal para uso

    // caso de atribuição de diretiva
    if(n->children[0]->type == ASSIGN_TDIRECTIVE)
    {
        // verificação semântica (se pode atribuir)(essas atribuições só são válidas no programa principal) ? seria uma boa tratar na gramática (?)
        if(scope->type == FUNC)
        {
            fprintf(stderr, "ERROR: BAD USE OF %s TIME DIRECTIVE, THE CONTEXT IS LOCKED! \n", n->children[0]->leafs[0]);
            exit(-1);
        }
        // dois casos: alterar o valor na tabela de simbolo
        // Mudou intervalos: alterar intervalo no main do SMV e mudar o init ou next (dependendo da diretiva)
        // Mudou CONTEXTO (C_TIME) : seguir o caso default

        // objeto sintetizado (chamar fora ou dentro do if depois das validações? avisa erros mais rapido)
        sintExpr = eval(n->children[1], scope, controllerSmv);
        expr = sintExpr[0];
        if(expr && expr->type != NUMBER_ENTRY && expr->OBJECT_SIZE > 1)
        {
            fprintf(stderr, "ERROR: BAD USE OF %s TIME DIRECTIVE, ONLY NUMERICAL VALUES ARE ACCEPTED \n", n->children[0]->leafs[0]);
            exit(-1);
        }
        if(*(int*)expr->values[0] < ctime ){
            fprintf(stderr, "ERROR: BAD USE OF %s TIME DIRECTIVE, IMPOSSIBLE TO RETURN TO PREVIOUS TIME CONTEXTS  \n", n->children[0]->leafs[0]);
            exit(-1);
        }
        // validaçao de intervalo
        TableEntry* itimeEntry = lookup(scope,"F_TIME");
        int ftime = *(int*)itimeEntry->val->values[0];
        if(ctime > ftime){
            fprintf(stderr, "WARNING: %s IS BEYOND THE OBSERVATION INTERVAL \n", n->children[0]->leafs[0]);
        }
        /* TOMAR NOTA: NUNCA MAIS FAZER ISSO
         * 		int* v;
         *       v = 5;  (PODE LITERALMENTE ALOCAR QUALQUER, REPITO QUALQUER REGIÃO DE MEMÓRIA PARA MEU 5!
         *       void* vp[] = {v};
         * */
        // só fazer isso se eu tiver dado malloc em v!
        void* vp[] = {expr->values[0]};
        updateValue(n->children[0]->leafs[0], vp, T_DIRECTIVE_ENTRY, 1, -1, -1, scope, 0);
        letgoObject(expr,0);
    }
    else{
        // busca expressão
        sintExpr = eval(n->children[1], scope, controllerSmv);
        expr = sintExpr[0];

        // busca a variável e seu contexto
        char* varName = n->children[0]->leafs[0];
        TableEntry* varEntry = lookup(scope,varName);
        Object* var = varEntry == NULL ?  NULL : varEntry->val;

        // decidir o que vai usar do Hcontroller (tem que ser melhor adaptado) (foi separado em métodos
        //STable* refAuxTable = accessSmvInfo(controllerSmv,scope->type == FUNC && expr->type == TDS_ENTRY? PORTS : MAIN);
        STable* refAuxTable = selectSMV_SCOPE(scope,controllerSmv);
        HeaderSmv* refHeader = selectSMV_INFO(scope,expr->type == FUNCTION_ENTRY? expr : NULL,controllerSmv);
        //HeaderSmv* refHeader = accessHeader(controllerSmv,scope->type == FUNC && expr->type == TDS_ENTRY? PORTS : MAIN ,scope->order);

        TableEntry* itimeEntry = lookup(scope,"I_TIME");
        int itime = *(int*)itimeEntry->val->values[0];
        int changeContext = ctime > itime; // verifica se mudou o contexto

        //binds da expressão
        // SINTH_BIND do valor da expressão (ou com o valor dela ou com o smv SINTH_BIND)
        char valueBind[300];
        // SINTH_BIND do valor default caso a atribuição venha a necessitar criar um init e next juntos
        char defaultValueBind[300];
        char* conditionCube = NULL ;

        // atribuição simples
        if(n->children[0]->type == ASSIGN_IDVAR)
        {
            //primeira vez da variavel
            if(!var)
            {
                conditionCube = formatBinds(ctime,changeContext,defaultValueBind,valueBind,defaultValueBind,expr,scope,1);
                addValue(varName, expr->values, expr->type, expr->OBJECT_SIZE, 0, scope, ctime);
                //inicialização "com next", necessita criar um default para os instantes anteriores e o seu next
                // note que temporal condition tem que ser um cubo de condição e tempo
                if(changeContext){
                    specAssign(varName, refHeader, refAuxTable, defaultValueBind, NULL, defaultValueBind,
                               0, NULL, scope->order, scope->level, expr->type, 0, NULL);
                    specAssign(varName, refHeader, refAuxTable, valueBind, conditionCube, NULL,
                               0, NULL, scope->order, scope->level, expr->type, 1, expr->values[0]);

                }
                else{
                    // condition em INIT? e default, deve-se criar default tmb
                    specAssign(varName, refHeader, refAuxTable, valueBind, conditionCube, defaultValueBind,
                               0, NULL, scope->order, scope->level, expr->type, 0, expr->values[0]);
                }
            }
            else{

                int prevDef = var->redef;
                int prevContext = var->timeContext;
                updateValue(varName, expr->values, expr->type, expr->OBJECT_SIZE, -1, -1, scope, ctime);
                // tempo > 0 e não ocorreu redefinição
                if(changeContext && var->redef == prevDef){
                    conditionCube = formatBinds(ctime,changeContext,defaultValueBind,valueBind,defaultValueBind,expr,scope, prevContext? 0 : 1);
                    specAssign(varName, refHeader, refAuxTable, valueBind, conditionCube, NULL,
                               var->redef, NULL, scope->order, scope->level, expr->type, 1, expr->values[0]);
                }
                else{
                    // tempo = 0, redefinição
                    if(!changeContext){
                        conditionCube = formatBinds(ctime,changeContext,defaultValueBind,valueBind,defaultValueBind,expr,scope,1);
                        specAssign(varName, refHeader, refAuxTable, valueBind, conditionCube, NULL,
                                   var->redef, NULL, scope->order, scope->level, expr->type, 0, expr->values[0]);
                    }
                    // tempo > 0 e redefinição
                    else{
                        conditionCube = formatBinds(ctime,changeContext,defaultValueBind,valueBind,defaultValueBind,expr,scope,1);
                        specAssign(varName, refHeader, refAuxTable, defaultValueBind, NULL, defaultValueBind,
                                   var->redef, NULL, scope->order, scope->level, expr->type, 0, expr->values[0]);
                        specAssign(varName, refHeader, refAuxTable, valueBind, conditionCube, NULL,
                                   var->redef, NULL, scope->order, scope->level, expr->type, 1, expr->values[0]);
                    }
                }
            }
        }
        else{
            if(var && expr->type != var->type){
                fprintf(stderr, "TYPE ERROR: %s datastructure type is imutable \n",varName);
                exit(-1);
            }

            printf("[evalOTHER_ASSIGN] atribui variável (atributo ou indice) \n");
            if(!var)
            {
                fprintf(stderr, "ERROR: %s is not defined as datastruct \n",varName);
                exit(-1);
            }

            Node* ref = n->children[0]->children[0];
            Object* indexRef = ref->type == V_PROP ? NULL : eval(n, scope, controllerSmv)[0];

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
        }

    }
    free(sintExpr);
    return NULL;
}

Object * evalCMD_IF(Node* n, STable* scope, HeaderController* controllerSmv){

    Object* conditionalExpr = NULL;
    Object** sintExpr = NULL;

    STable* IF_SCOPE = addSubScope(scope,IF_BLOCK);
    sintExpr = eval(n->children[0], scope, controllerSmv);
    conditionalExpr = sintExpr[0];
    printObject(conditionalExpr);
    bindCondition(IF_SCOPE,conditionalExpr);
    if(*(int*)conditionalExpr->values[0]){

        eval(n->children[1], IF_SCOPE, controllerSmv); // tenho que passar um parâmetro que define se avaliou
    }
    else{
        if(n->children[1]){
            // cria else
        }
    }
    return NULL;
}

Object * evalMATCH_IF(Node* n, STable* scope, HeaderController* controllerSmv){

}


Object **eval(Node *n, STable *scope, HeaderController *controllerSmv)
{
    printf("[eval] %s \n",n->name);
    if(n)
    {
        // sintetizado dos filhos
        //void** SYNTH_C[n->nchild];
        // sintetizado dos filhos
        //void** SYNTH_L[n->nleafs];
        Object ** SYNTH_O = malloc(n->nchild*sizeof(Object*));

        if(executores[n->type])
        {
            printf("[PostProcess - eval] eval especifico \n\n");
            SYNTH_O[0] = executores[n->type](n,scope,controllerSmv);
        }
        else
        {
            printf("[PostProcess - eval] eval genérico \n");
            if(n->children)
            {
                int i;
                for(i= 0; i < n->nchild; i++)
                {
                    Node* toEval = n->children[i];
                    if(toEval)
                    {
                        printf("(%d) %s \n",i,toEval->name);
                        Object** SYNTH_SET = eval(n->children[i], scope, controllerSmv);
                        SYNTH_O[i] = SYNTH_SET[0];
                        free(SYNTH_SET);
                    }
                    // (já criamos Object) resolver dependencias realmente necessário? Parando para pensar podemos acessar o filho imediatamente abaixo do nó em questão e já pegar os valores ! Evita criar mais structs! (pode ficar complexo para alguns casos por outro lado... e fora que inviabiliza tds e vetores)
                }
            }
        }
        return SYNTH_O;
    }
    return NULL;
}


