//
// Created by mateus on 28/10/2020.
//

#ifndef INTERP_H

#define INTERP_H

#include "textManager.h"
#include "HeaderSmv.h"
#include "STable.h"
#include "Node.h"
#include "Object.h"

Object *eval(Node *n, STable *scope, HeaderController *controllerSmv);

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


#endif

