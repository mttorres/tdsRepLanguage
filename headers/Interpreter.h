//
// Created by mateus on 28/10/2020.
//

#ifndef INTERP_H

#define INTERP_H

#include "LangCore.h"

typedef enum MAP_OP { PLUS = 43, MINUS = 45, TIMES = 42, DIVIDE = 47, MOD = 37, LT = 60, GT = 62, NOTEQUAL = 94, NOT_PREFIX = 110,
    LE = 121, EQUAL = 122, GE = 123} MAP_OP;

/**
 * Inicia o processo de interpretação. E ao final commita todos os instantes restantes para uma TDS.
 * @param n o nó raiz da AST
 * @param scope o escopo GLOBAL
 * @param controller o controlador de ambiente
 */
void startInterpreter(Node* n, STable* scope, EnvController* controller);

#endif

