//
// Created by mateus on 22/03/2021.
//

#ifndef TDSREPLANGUAGE_VISITOR_H
#define TDSREPLANGUAGE_VISITOR_H

#include "PostProcess.h"
#include "Node.h"
#include "Object.h"

Object* evalNUM(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalBOOL(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalSTRING(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalNULL(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalIDVAR(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalTIME_DIRECTIVE(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalDataV(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalPARAMS(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalPARAMS_CALL(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalAC_V(Node* n, STable* scope, EnvController* controllerSmv);
Object* eval_ASSIGN_TDIRECTIVE(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalOTHER_ASSIGN(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalV_PROP(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalADD_V(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalADD_V_PROP(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalV_PROP_TDS(Node* n, STable* scope, EnvController* controllerSmv);
Object * evalEXPR(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalMINUS(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalPLUS(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalMULTI(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalDIV(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalLE(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalGE(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalLT(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalGT(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalEQUAL(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalNEQUAL(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalNOT(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalAND(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalOR(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalIMP(Node* n, STable* scope, EnvController* controller);
Object* evalBIMP(Node* n, STable* scope, EnvController* controller);
Object* evalPRI_EXPR(Node* n, STable* scope, EnvController* controllerSmv);
Object * evalDEFINE_INTERVAL(Node* n, STable* scope, EnvController* controllerSmv);
Object * evalCMD_IF(Node* n, STable* scope, EnvController* controllerSmv);
Object * evalTDS_DEF_COMPLETE(Node* n, STable* scope, EnvController* controllerSmv);
Object * evalTDS_DEF_DEPENDECE(Node* n, STable* scope, EnvController* controllerSmv);
Object * evalTDS_DATA_TIME_COMPONENT(Node* n, STable* scope, EnvController* controllerSmv);
Object * evalANON_TDS(Node* n, STable* scope, EnvController* controllerSmv);
Object* eval_ITERATOR(Node* n, STable* scope, EnvController* controllerSmv);
Object* evalV_PROP_TDS_VALUE(Node* n, STable* scope, EnvController* controllerSmv);

Object* (* *executores) (Node* n, STable* scope, EnvController* controllerSmv);

/**
 * Função mapeadora de avaliações do interpretador. De forma que essa função busca
 * a avaliação apropriada e repassa para o interpretador.
 * @param n o nó corrente da AST
 * @param scope o escopo corrente
 * @param controllerSmv o controlador de ambiente
 * @return retorna um objeto genérico resultado da interpretação (OBJETO SINTETIZADO)
 */
Object *eval(Node *n, STable *scope, EnvController *controllerSmv);

// usar depois para criar executores genericos
// caso a gente queira simplificar os parâmetros (note que muitos são desnecessários em alguns evals (MELHORIA))
//typedef Object* (*generic_fp)(void);
/*
 * generic_fp executores[80] = {
        (generic_fp ) evalNUM, (generic_fp ) evalBOOL, (generic_fp ) evalSTRING, (generic_fp ) evalNULL, (generic_fp ) evalIDVAR, (generic_fp ) evalTIME_DIRECTIVE,
        (generic_fp ) evalDataV, (generic_fp ) evalPARAMS_CALL, (generic_fp ) evalDEFINE_INTERVAL , (generic_fp ) evalAC_V,
        (generic_fp ) evalOTHER_ASSIGN, (generic_fp ) evalV_PROP, (generic_fp ) evalADD_V, (generic_fp ) evalADD_V_PROP, (generic_fp ) evalV_PROP_TDS, (generic_fp ) evalEXPR, (generic_fp )
        (generic_fp ) evalCMD_IF, (generic_fp ) evalMATCH_IF
};
 * */


#endif //TDSREPLANGUAGE_VISITOR_H
