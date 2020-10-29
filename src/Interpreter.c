//
// Created by mateus on 28/10/2020.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../headers/PostProcess.h"


typedef enum MAP_OP { PLUS = 43, MINUS = 45, TIMES = 42, DIVIDE = 47, MOD = 37, LT = 60, GT = 62, NOTEQUAL = 94,
    LE = 121, EQUAL = 122, GE = 123} MAP_OP;


Object* evalNUM(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
{
    printf("[evalNUM] \n");
    int* sint;
    *sint = atoi(n->leafs[0]);


    printf("[evalNUM] SINTH: %d \n",*sint);

    void* ip[] = {sint};

    Object* o = createObject(NUMBER_ENTRY, 1, ip);


    return o;
}

Object* evalBOOL(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
{
    printf("[evalBOOL] \n");
    int* sint;

    char* trueString = "true";

    *sint= strstr(n->leafs[0],trueString)? 1 : 0;

    printf("[evalBOOL] SINTH: %d \n",*sint);

    void* bp[] = {sint};

    Object* o = createObject(LOGICAL_ENTRY, 1, bp);

    return o;
}


Object* evalSTRING(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
{
    printf("[evalSTRING] \n");
    char* sint =  n->leafs[0];

    void* sp[] = {sint};

    printf("[evalSTRING] SINTH: %s \n",sint);

    Object* o = createObject(LABEL_ENTRY, 1, sp);

    return o;
}


/*
	Pensar depois... encapsular NULL ?

*/

Object* evalNULL(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
{
    printf("[evalNULL] \n");
    // se eu interpretar como "NULL" do C mesmo podemos ter problemas(?)
    char* sint =  n->leafs[0];

    void* np[] = {sint};

    printf("[evalNULL] SINTH: %s \n",sint);

    Object* o = createObject(NULL_ENTRY, 1, np);

    return o;
}


Object* evalIDVAR(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
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


Object* evalTIME_DIRECTIVE(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
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
        return createObject(NUMBER_ENTRY,1,entry->val->values);
    }
}



Object* evalDataV(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
{
    printf("[evalDataV] \n");


    //str= evalSTRING(n,  scope, writeSmvTypeTable, controllerSmv);

    //printf("[evalDataV] DATA_V SINTH: %s \n",str);

    //return sint;

    // esse vai ser bem diferente...

    // vai chamar evalParams , e sintetizar um Object Vetor (ou um vetor void que será jogado em um object)
}

Object* evalPARAMS_CALL(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
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
        return createObject(NUMBER_ENTRY,1,rp);
    }
    if(o1->type == LABEL_ENTRY)
    {
        // concatena

    }

}

Object* evalMINUS(Object* o1, Object* o2)
{
    printf("[evalMINUS] \n");
    int* r;
    *r = (*(int*)o1->values[0]) - (*(int*)o2->values[0]);
    void* rp[] = {r};
    return createObject(NUMBER_ENTRY,1,rp);
}

Object* evalTIMES(Object* o1, Object* o2)
{
    int* r;
    *r = (*(int*)o1->values[0]) * (*(int*)o2->values[0]);
    void* rp[] = {r};
    return createObject(NUMBER_ENTRY,1,rp);
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

    return createObject(NUMBER_ENTRY,1,rp);
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
    return createObject(LOGICAL_ENTRY,1,rp);
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
    return createObject(LOGICAL_ENTRY,1,rp);

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



Object* evalEXPR(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
{
    printf("[evalEXPR] \n");
    // operação unária
    if(n->nchild == 1)
    {

    }
        // operação binária
    else
    {
        // CUIDADO (ordem avaliação)
        Object* o1 = evalEXPR(n->children[0], scope, writeSmvTypeTable, controllerSmv);
        Object* o2 = evalEXPR(n->children[1],scope, writeSmvTypeTable, controllerSmv);

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


Object* evalProp(Node* fatherRef, Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
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
        Object* expr = evalEXPR(n->children[0],scope, writeSmvTypeTable, controllerSmv);
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


Object* evalAC_V(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
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
        Object* prop = evalProp(n,n->children[0], scope, writeSmvTypeTable, controllerSmv);

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


Object* evalOTHER_ASSIGN(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
{
    if(n->children[0]->type == ASSIGN_TDIRECTIVE)
    {

        // verificação semântica (se pode atribuir)(essas atribuições só são válidas no programa principal)
        // seria uma boa tratar na gramática (?)
        if(scope->type == FUNC)
        {
            fprintf(stderr, "ERROR: BAD USE OF %s TIME DIRECTIVE, THE CONTEXT IS LOCKED! \n", n->children[0]->leafs[0]);
            exit(-1);
        }

        // dois casos: alterar o valor na tabela de simbolos
        //Mudou intervalos: alterar intervalo no main do SMV e mudar o init ou next (dependendo da diretiva)
        //Mudou CONTEXTO (C_TIME) : seguir o caso default

        // objeto sintetizado
        Object* expr = NULL;
        if(expr && expr->type != NUMBER_ENTRY && expr->OBJECT_SIZE > 1)
        {
            fprintf(stderr, "ERROR: BAD USE OF %s TIME DIRECTIVE, ONLY NUMERICAL VALUES ARE ACCEPTED \n", n->children[0]->leafs[0]);
            exit(-1);
        }

        // TOMAR NOTA: NUNCA MAIS FAZER ISSO
        /*
         * 		int* v;
         *       v = 5;  (PODE LITERALMENTE ALOCAR QUALQUER, REPITO QUALQUER REGIÃO DE MEMÓRIA PARA MEU 5!
         *       void* vp[] = {v};
         * */
        // só fazer isso se eu tiver dado malloc em v!
        int v;
        v = 5;
        void* vp[] = {&v};
        updateValue(n->children[0]->leafs[0], vp, T_DIRECTIVE_ENTRY, 1, 0, 0, scope);
/*
		char iniITIME = 'I';
		if(n->children[0]->leafs[0] == iniITIME)
		{
			writeSmvTypeTable[0];
		}
		else
		{

		}
*/
    }
    //printf("exit eval \n");
    //printTable(scope);
    return NULL;
}



// declarar com const ? (me parece "nada haver")
Object* (*executores[80]) (Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv) = {

        evalNUM, evalBOOL, evalSTRING, evalNULL, evalIDVAR, evalTIME_DIRECTIVE, evalDataV, evalPARAMS_CALL ,evalAC_V,
        evalOTHER_ASSIGN


};



void eval(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv)
{
    printf("[eval] %s \n",n->name);
    if(n)
    {
        // sintetizado dos filhos
        //void** SYNTH_C[n->nchild];
        // sintetizado dos filhos
        //void** SYNTH_L[n->nleafs];

        if(executores[n->type])
        {
            printf("[PostProcess - eval] eval especifico \n\n");
            executores[n->type](n,scope,writeSmvTypeTable,controllerSmv);
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
                        eval(n->children[i],scope,writeSmvTypeTable,controllerSmv);
                        printf("???\n");
                        printf("referencia!? %d \n",toEval == NULL);
                        printf("(saida?) %s \n",n->name);
                        printf("(saida) %s \n",toEval->name);
                    }
                    printf("exit if %d \n",i);
                    //printf("exit if %d \n",i);														 // (já criamos Object) resolver dependencias realmente necessário? Parando para pensar podemos acessar o filho imediatamente abaixo do nó em questão e já pegar os valores ! Evita criar mais structs! (pode ficar complexo para alguns casos por outro lado... e fora que inviabiliza tds e vetores)
                }
            }
        }
    }
    printf("[eval] POP \n");
}


