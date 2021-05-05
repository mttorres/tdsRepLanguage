//
// Created by mateus on 22/03/2021.
//
#include <stdio.h>
#include <string.h>
#include "../headers/Visitor.h"

Object* (*realizacaoAvaliaco[160]) (Node* n, STable* scope, EnvController* controllerSmv) = {
        evalNUM, evalBOOL, evalSTRING, evalNULL, evalIDVAR, evalTIME_DIRECTIVE,
        evalDataV, evalPARAMS_CALL, evalPARAMS ,evalDEFINE_INTERVAL ,evalAC_V,
        eval_ASSIGN_TDIRECTIVE, evalOTHER_ASSIGN, evalV_PROP, evalADD_V, evalADD_V_PROP, evalV_PROP_TDS, evalEXPR,
        evalMINUS, evalPLUS, evalMULTI, evalDIV, evalLE, evalGE, evalLT, evalGT, evalEQUAL, evalNEQUAL, evalNOT, evalAND, evalOR, evalIMP, evalBIMP,
        evalPRI_EXPR, evalCMD_IF,
        evalTDS_DEF_COMPLETE, evalTDS_DEF_DEPENDECE, evalANON_TDS, eval_ITERATOR, evalTDS_DATA_TIME_COMPONENT,
        evalV_PROP_TDS_VALUE
};

Object* (* *executores) (Node* n, STable* scope, EnvController* controllerSmv) = realizacaoAvaliaco;


Object *eval(Node *n, STable *scope, EnvController *controllerSmv)
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
            //printf("[PostProcess - eval] eval especifico \n\n");
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
        }
        return SYNTH_O;
    }
    return NULL;
}


