// Created by mateus on 22/03/2021.
//

#ifndef TDSREPLANGUAGE_ENVCONTROLLER_H
#define TDSREPLANGUAGE_ENVCONTROLLER_H

#include "Enum.h"
#include "STable.h"
#include "textManager.h"
#include "HeaderSmv.h"
#include "TDS.h"
#include "TypeSet.h"
#include "TypeMinMax.h"

#define TYPE_SET_DIR_SIZE 9999
#define DEFAULT_HEADERS_AUX_SIZE 100



typedef struct envcontroller
{
    // info
    HeaderSmv* MAIN;
    HeaderSmv** AUTOMATA_RELATED;
    HeaderSmv** PORTS_RELATED; // ports module e as tds (vão ter referência para esse header na sua declaração, assim como tabela auxiliar)
    HeaderSmv** FUNCTIONS; // as funções usadas por main e tds (vão ter referência para seu header na sua declaração, assim como tabela auxiliar)

    int H_FUNCTION_CURRENT_SIZE;
    int H_AUTOMATA_CURRENT_SIZE;
    int H_PORTS_CURRENT_SIZE;
    int PORTS_INFO_CURRENT_SIZE;

    STable* mainInfo; // tabela de simbolos auxiliar para main
    STable** portsInfo; // para cada tds (talvez não va precisar, a lógica vai estar TODA nas funções ou nele mesmo (de maneira constante)
    STable** functionsInfo; // para cada função

    char** typeSetWords; // dict para "otmização"

    //contexto tds
    TDS** declaredPorts;
    TDS* currentTDScontext;



    // validação
    STable* originalPorts;
    int expectedPorts;
    int validPorts;
    // ai ele verifica se foram usadas (não tem declarações de funções)
    int declaredPortsNumber;
    // avisa caso não tenha nenhuma TDS linkada uma com a outra
    int IO_RELATION;
    // avisa caso uma porta tenha tido declaração repetida
    int multiPortDeclartion;

    // validações para o filter e recuperar o header correto
    int modelHasFilter;
    int* automatasToChange;
    int filterContext;
    int F_AUTOMATAS_CHANGE_POINTER;
    int modelHasFinalAutomata;
    int filterUsed;

    // interactive
    int interactiveMode;

}EnvController;


EnvController *createController(int enableInteractive);

/**
 * Libera o controlador e todas as estruturas armazenadas por ele
 * @SideEffects: Chama o liberar dos headers e  tabelas de simbolos
 * */
void letGoHeaderControl(EnvController* Hcontrol);

/**
 * Printa todas os headers armazenados pelo controlador
 * */
void printAllHeaders(EnvController* Hcontrol);

/**
* Considerando a refatoração do controller e header, encapsulamos o header apropriado pela estrutura, e esse deve
* ser recuperado usando o enum de smvtype e o indice.
*
* Caso o indice não seja informado o último header salvo (tanto do agrupamento main, automato, ou ports) é retornado
* Esse comportamento é usado pelo pré processamento, considerando que cada agrupamento é consultado para verificar o
        * progresso de algumas alterações (ex: trocar FROZENVAR de portsModule), e otimizações da leitura
*
* @param controller o controlador de headers
* @param cat o tipo smv de parte de intesse (main, automato/modulo generico, ports)
* @param indexOfHeader a ordem de headers de cada parte de interesse (ex: 0 = portsModule, 1: função1 associada a tds)
*
* @returns o header apropriado com a parte de interesse do arquivo SMV
* */
HeaderSmv* accessHeader(EnvController* controller, smvtype cat, int indexOfHeader);

/**
 * Considerando a refatoração do controller e header, encapsulamos as tabelas de simbolo auxiliares (SMV_INFO)
 * ser recuperado usando o enum de smvtype.
 *
 * @param controller o controlador de headers
 * @param cat o tipo smv de parte de intesse (main, automato/modulo generico, funções)
 * @param SMV_INFO_ID o identificador daquela tabela auxiliar (usado atualmente só para as funções)
 * @returns a tabela de simbolos SMV_INFO apropriada
 * */
STable* accessSmvInfo(EnvController* controller, smvtype cat, int SMV_INFO_ID);

/**
 * Adiciona um novo header ao controller encapsulando suas operações e categorizando apropriadamente
 * @param o controller
 * @param o novo header retornado por createHeader
 */
void addNewHeader(EnvController* controller, HeaderSmv* newHeader);

/**
 * Adiciona uma nova tabela de simbolos auxiliar ao controller encapsulando suas operações e categorizando apropriadamente
 * @param o controller
 * @param a nova tabela de simbolos auxiliar retornada por retornado por createTable
 */
void addNewAuxInfo(EnvController* controller, STable* newTableInfo);

/**
 * Valida a declaração de uma TDS, vendo se os nomes bateram com os que vieram do modelo de input.
 * @param declarationName o nome da declaração da TDS
 * @param controller o controlador
 */
void validateTdsDeclaration(char* declarationName, EnvController* controller);

/**
 * Propaga um parâmetro para o mpodulo de uma TDS recuperando os headers apropriados dessa.
 * @param controller o controlador de ambiente
 * @param param o identificador do parametro
 * @param currentTDS a tds recuperada da interpretação
 */
void addParamToTdsDeclaration(EnvController* controller, char* param, TDS* currentTDS);

/**
 * Tendo um controller com o contexto e associação Header e AuxTable,
 * adiciona um parâmetro (se necessário) para o header do módulo da TDS.
 * Também propaga as dependências relacionadas a esse parâmetro para o portsModule
 * @param controller o controlador de ambiente
 * @param param o bind daquele parâmetro
 * @param currentTDS a TDS corrente
 * @SideEffects: Aloca uma string nova, caso esse parâmetro já não tenha sido memoizado
 */
void addParamToTds(EnvController* controller, char* param, TDS* currentTDS);

/**
 * Adiciona um novo parâmetro a portModule, e resolve as depêndencias nos demais módulos main e automato.
 * @param o controller
 * @param o novo parâmetro
 * @SideEffects: Para cada módulo (main, automato ...) é atualizada a linha com depdência a ports e essa operação chama
 * o módulo textManager e seus método addParams, criando uma string nova e liberando a antiga
 */
void addParamToPortsModule(EnvController *controller, char *param);

/**
 * Recupera uma palavra do dicionário de type-set do ambiente.
 * @param word uma palavra já utilizada anteriomente em um type-set
 * @param controller o controlador de ambiente
 * @return um ponteiro para a palavra, ou NULL caso essa não exista
 */
char* getTypeSetWordFromDict(char* wordRef, EnvController* controller);

/**
 * Adiciona uma palavra ao dicionário de type-set do ambiente
 * @param word a palavra nova
 * @param controller o controlador
 * @SideEffects: Aloca uma string nova de tamanho da word +1 (deve ser liberada no final do programa)
 */
void addTypeSetWordToDict(char* word, EnvController* controller);

/**
 * Atualizaa os parâmetros de todos os automatos que possam ter condição de filtro. E propaga os parâmetros
 * @param controller o controlador
 * @param paramName o parâmetro
 */
void addParamToAutomatasFilter(EnvController* controller, char* paramName);

/**
 * Realiza diversas validações de contexto.
 * @param controller o controller que contem todas as informações de necessárias
 */
void validateAfterInterPost(EnvController* controller);


#endif //TDSREPLANGUAGE_ENVCONTROLLER_H