//
// Created by mateus on 06/01/2021.
//

#ifndef TDSREPLANGUAGE_INTERPRETEREVALS_H
#define TDSREPLANGUAGE_INTERPRETEREVALS_H


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

Object **eval(Node *n, STable *scope, HeaderController *controllerSmv);
Object* evalNUM(Node* n, STable* scope, HeaderController* controllerSmv);
Object* evalBOOL(Node* n, STable* scope, HeaderController* controllerSmv);
Object* evalSTRING(Node* n, STable* scope, HeaderController* controllerSmv);
Object* evalNULL(Node* n, STable* scope, HeaderController* controllerSmv);
Object* evalIDVAR(Node* n, STable* scope, HeaderController* controllerSmv);
Object* evalTIME_DIRECTIVE(Node* n, STable* scope, HeaderController* controllerSmv);
Object* evalDataV(Node* n, STable* scope, HeaderController* controllerSmv);
Object* evalPARAMS_CALL(Node* n, STable* scope, HeaderController* controllerSmv);
Object* evalAC_V(Node* n, STable* scope, HeaderController* controllerSmv);
Object* evalOTHER_ASSIGN(Node* n, STable* scope, HeaderController* controllerSmv);
Object* evalV_PROP(Node* n, STable* scope, HeaderController* controllerSmv);
Object* evalADD_V(Node* n, STable* scope, HeaderController* controllerSmv);
Object* evalADD_V_PROP(Node* n, STable* scope, HeaderController* controllerSmv);
Object* evalV_PROP_TDS(Node* n, STable* scope, HeaderController* controllerSmv);
Object * evalEXPR(Node* n, STable* scope, HeaderController* controllerSmv);
Object * evalDEFINE_INTERVAL(Node* n, STable* scope, HeaderController* controllerSmv);
Object * evalCMD_IF(Node* n, STable* scope, HeaderController* controllerSmv);
Object * evalMATCH_IF(Node* n, STable* scope, HeaderController* controllerSmv);

#endif //TDSREPLANGUAGE_INTERPRETEREVALS_H
