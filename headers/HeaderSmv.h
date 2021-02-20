#ifndef HEADER_H

#define HEADER_H

#include "Enum.h"
#include "STable.h"
#include "textManager.h"
#include "TDS.h"

typedef enum headerpart {CREATE_MODULE = -1, VAR = 0, ASSIGN = 1, TRANS = 2} headerpart;

typedef struct headersmv
{
  smvtype type;
  char* moduleName;
  char** varBuffer; // poderia transormar em uma tabela hash(facilidade em achar a variavel, mas acredito que uma vez escrito aqui não vai ter alteração!)
  char** transBuffer; // vai ser inalterado durante a execução (só no pré processamento, ou seja já é criado preeenchido)
  char** assignBuffer; // aqui a ordem é importante não pode ser uma tabela hash
  int VAR_POINTER;
  int TRANS_POINTER;
  int ASSIGN_POINTER;
  int VAR_RENAME_POINTER; // aponta para a referência a portsModule que vai ser passada para os demais módulos (e será centralizada no main agora)

}HeaderSmv;

HeaderSmv* createHeader(int type, char* moduleName, int varP, int assignP, int transP);

void letgoHeader(HeaderSmv* h);


typedef struct envcontroller
{
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

    STable* originalPorts; // tabela de simbolos auxiliar para ports (necessária?) (só vai ter o módulo de cada porta) (pode ser na verdade para as portas declaradas)
    int expectedPorts;
    int validPorts;
    // ai ele verifica se foram usadas (não tem declarações de funções)
    int declaredPortsNumber;
    TDS** declaredPorts;


}HeaderController;


HeaderController *createController();

/**
 * Libera o controlador e todas as estruturas armazenadas por ele
 * @SideEffects: Chama o liberar dos headers e  tabelas de simbolos
 * */
void letGoHeaderControl(HeaderController* Hcontrol);

/**
 * Printa cada componente: VAR, ASSIGN, TRANS de um módulo SMV
 * */
void printHeaderBuffer(HeaderSmv* h, int type, char* typeString);

/**
 * Printa o módulo SMV e suas componentes: VAR, ASSIGN, TRANS
 * */
void printHeader(HeaderSmv* h);

/**
 * Printa todas os headers armazenados pelo controlador
 * */
void printAllHeaders(HeaderController* Hcontrol);

void writeHeaderBuffer(HeaderSmv* h, int part, FILE* smvoutput);

void writeHeader(HeaderSmv* header, FILE* smvoutput);

void writeHeaderBuffer(HeaderSmv* h, int part, FILE* smvoutput);

void writeHeader(HeaderSmv* header, FILE* smvoutput);

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
HeaderSmv* accessHeader(HeaderController* controller, smvtype cat, int indexOfHeader);

/**
 * Considerando a refatoração do controller e header, encapsulamos as tabelas de simbolo auxiliares (SMV_INFO)
 * ser recuperado usando o enum de smvtype.
 *
 * @param controller o controlador de headers
 * @param cat o tipo smv de parte de intesse (main, automato/modulo generico, funções)
 * @param SMV_INFO_ID o identificador daquela tabela auxiliar (usado atualmente só para as funções)
 * @returns a tabela de simbolos SMV_INFO apropriada
 * */
STable* accessSmvInfo(HeaderController* controller, smvtype cat, int SMV_INFO_ID);

/**
 * Adiciona um novo header ao controller encapsulando suas operações e categorizando apropriadamente
 * @param o controller
 * @param o novo header retornado por createHeader
 */
void addNewHeader(HeaderController* controller, HeaderSmv* newHeader);

/**
 * Adiciona uma nova tabela de simbolos auxiliar ao controller encapsulando suas operações e categorizando apropriadamente
 * @param o controller
 * @param a nova tabela de simbolos auxiliar retornada por retornado por createTable
 */
void addNewAuxInfo(HeaderController* controller, STable* newTableInfo);

/*
	escolhe um buffer do header atual para salvar a linha (alocando sempre uma string para essa linha, que depois deve ser liberada).
	efeitos colaterais:  * ao ter readVarPortsModule como true, ele salva as variáveis do portsModule em uma tabela de portas
						 * ao ter controlRename como true, ele remove todas as ocorrências de determinados caracteres (no caso [])

*/
void selectBuffer(headerpart part, char* line, HeaderSmv* header, int controlRename);


/**
 * Adiciona um novo parâmetro a portModule, e resolve as depêndencias nos demais módulos main e automato.
 * @param o controller
 * @param o novo parâmetro
 * @SideEffects: Para cada módulo (main, automato ...) é atualizada a linha com depdência a ports e essa operação chama
 * o módulo textManager e seus método addParams, criando uma string nova e liberando a antiga
 */
void addParamToPortsModule(HeaderController *controller, char *param, int first);

#endif
