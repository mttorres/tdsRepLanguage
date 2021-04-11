//
// Created by mateus on 28/10/2020.
//

#ifndef INTERP_H

#define INTERP_H

#include "Visitor.h"

//#define DEFAULT_MEMOI 300


//Object** MEMOI;


/*
 * FUNÇÕES AUXILIARES
 */

STable* selectSMV_SCOPE(STable* scope, EnvController* controllerSmv);

HeaderSmv * selectSMV_INFO(STable* scope, Object* functionPointer, EnvController* controllerSmv);
// avaliar a importancia dos métodos acima, e se possível movimentar eles para o HeaderSmv.c ou para um Novo Controller.h

/**
 * Resolve a dependencia de todas as TDS's linkadas a TDS corrente durante a avaliação lazy
 * @param currentTDS a TDS corrente
 * @param controllerSmv o controller usado para atualizar o type-set das TDS's dependentes.
 * @param C_TIME para indexar o data-time correto
 */
void resolveDependencies(TDS* currentTDS, EnvController* controllerSmv, int C_TIME );

/**
 * Resolve a avaliação lazy das TDS's pendentes após uma mudança de contexto temporal. E salva seus dados para Memoization.
 * @param currentScope o escopo atual
 * @param controllerSmv o controlador de ambiente
 * @param C_TIME o contexto temporal atual
 */
void resolveTdsLazyEvaluation(STable *currentScope, EnvController *controllerSmv, int C_TIME);

// CASO FORA DE FLUXO 1: e se ele "pular", ex: commitar c_time = 2 (quando era 0 antes), ele pulou o 1! A gente deve ver a "diferença"
// CHAMAR ESSA FUNÇÃO DIFERENÇA VEZES.
// CASO FORA DE FLUXO 2: deve chamar isso também ao FIM DO PROGRAMA (para cada CHAMADA RESTANTE, ex: C_TIME terminou em 4, deve chamar até F_TIME (5, ... , F_TIME).
// DIFERENÇA VEZES DE NOVO!
void commitCurrentTime(STable* currentScope, EnvController* controllerSmv, int changedTo);

/**
 * Real as operaçoes basicas envolvidas em quaisquer declaraçoes de TDS's. Dentre elas, alocaçao da estrutura,
 * consulta das diretivas temporais, sintetização de binds e, se necessário, resolução de dependências.
 * @param pathForDepen o path para se resolver uma dependência, isto é o dos nós mais abaixo da AST
 * @param portName o nome da porta
 * @param type o tipo da TDS para enumeraçao
 * @param tdsSpec o objeto que representa a especificaçao da TDS
 * @param delayed o parametro delayed de uma TDS
 * @param scope o scopo corrente
 * @param pathForCond o caminho para recuperar a condição a ser avaliada para essa TDS
 * @param controller o controlador de ambiente
 * @return um objeto sintetizado com a TDS criada.
 * @SideEffects: Aloca uma TDS, e posições no vetor de dependências de uma TDS se for necessário
 */
Object *computeTDSBasicOperations(Node *pathForDepen, char *portName, TDS_TYPE type, Object *tdsSpec, int delayed, STable *scope, Node *pathForCond, EnvController *controller);


/**
 *
 * @param n o nó passado para resolução de dependências
 * @param portName o nome da porta da tds
 * @param scope o escopo corrente
 * @param newTDS a nova TDS
 * @param controller o controlador de ambiente
 * @param I_TIME a diretiva de tempo inicial
 * @param C_TIME a diretiva de tempo corrente
 * @return a TDS com as dependencias computadas
 * @SideEffects: Aloca e posições no vetor de dependências de uma TDS se for necessário
 */
void computeTDSDependentOperations(Node*n, char* portName, STable* scope, TDS* newTDS, EnvController* controller, int I_TIME, int C_TIME);


/**
 * Resolve a avaliação lazy das TDS's pendentes após uma mudança de contexto temporal. E salva seus dados para Memoization.
 * @param currentScope o escopo atual
 * @param controllerSmv o controlador de ambiente
 * @param C_TIME o contexto temporal atual
 */
void resolveTdsLazyEvaluation(STable *currentScope, EnvController *controllerSmv, int C_TIME);


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

