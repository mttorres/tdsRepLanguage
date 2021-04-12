#ifndef HEADER_H

#define HEADER_H

#include "Enum.h"
#include "textManager.h"
#include "Hash.h"
#include <stdio.h>

#define MAX_PARAM 301

typedef enum headerpart {CREATE_MODULE = -1, VAR = 0, ASSIGN = 1, TRANS = 2} headerpart;

typedef struct headersmv
{
  smvtype type;
  char* moduleName;
  char** varBuffer; // poderia transormar em uma tabela calculateHashPos(facilidade em achar a variavel, mas acredito que uma vez escrito aqui não vai ter alteração!)
  char** transBuffer; // vai ser inalterado durante a execução (só no pré processamento, ou seja já é criado preeenchido)
  char** assignBuffer; // aqui a ordem é importante não pode ser uma tabela calculateHashPos
  int VAR_POINTER;
  int TRANS_POINTER;
  int ASSIGN_POINTER;
  int VAR_RENAME_POINTER; // aponta para a referência a portsModule que vai ser passada para os demais módulos (e será centralizada no main agora)
  int* PARAM_MAP;
  int expectFilter;
  int* filterPosNeg;
  int* filterPosCond;

}HeaderSmv;

/**
 * Cria um Header de módulo que será escrito depois no arquivo SMV.  Dependendo do tipo do header a ser escrito,
 * ele terá o nome do módulo passado com alguma alteração de parâmetro.
 * {MAIN = 1, AUTOMATA = 2, PORTS = 3, FUNCTION_SMV = 4}, onde para qualquer um exceto o main ele adiciona um parametro time ao modulo
 * @param type o tipo do header
 * @param moduleName o nome padrão do módulo da forma: MODULE %s\n
 * @param varP o ponteiro de controle para a parte var
 * @param assignP o ponteiro de controle para a parte assign
 * @param transP o ponteiro de controle para a parte trans
 * @param finalAutomataFilterModel usado para otimizar a verificação na hora que um automato filter é criado
 * @return O novo Header do novo módulo
 * @SideEffects: Aloaca um header que deve ser liberado como responsabilidade do chamador
 */

HeaderSmv * createHeader(enum smvtype type, char *moduleName, int varP, int assignP, int transP, int finalAutomataFilterModel);

void letgoHeader(HeaderSmv* h);

void letGoRelatedHeaders(HeaderSmv** RHeader, int size);

/**
 * Printa cada componente: VAR, ASSIGN, TRANS de um módulo SMV
 * */
void printHeaderBuffer(HeaderSmv* h, int type, char* typeString);

/**
 * Printa o módulo SMV e suas componentes: VAR, ASSIGN, TRANS
 * */
void printHeader(HeaderSmv* h);

void writeHeaderBuffer(HeaderSmv* h, int part, FILE* smvoutput);

void writeHeader(HeaderSmv* header, FILE* smvoutput);

void writeHeaderBuffer(HeaderSmv* h, int part, FILE* smvoutput);

void writeHeader(HeaderSmv* header, FILE* smvoutput);

/*
	escolhe um buffer do header atual para salvar a linha (alocando sempre uma string para essa linha, que depois deve ser liberada).
	efeitos colaterais:  * ao ter readVarPortsModule como true, ele salva as variáveis do portsModule em uma tabela de portas
						 * ao ter controlRename como true, ele remove todas as ocorrências de determinados caracteres (no caso [])

*/
void selectBuffer(headerpart part, char* line, HeaderSmv* header, int controlRename);

#endif
