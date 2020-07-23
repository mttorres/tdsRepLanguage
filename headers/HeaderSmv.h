#ifndef HEADER_H

#define HEADER_H


typedef struct headersmv
{
  int type;
  char* moduleName;
  char** varBuffer; // poderia transormar em uma tabela hash(facilidade em achar a variavel, mas acredito que uma vez escrito aqui não vai ter alteração!)
  char** transBuffer; // vai ser inalterado durante a execução (só no pré processamento, ou seja já é criado preeenchido)
  char** assignBuffer; // aqui a ordem é importante não pode ser uma tabela hash
  int VAR_POINTER;
  int TRANS_POINTER;
  int ASSIGN_POINTER;
  
}HeaderSmv;

HeaderSmv* createHeader(int type, char* moduleName, int varP, int assignP, int transP);

void printHeaderBuffer(HeaderSmv* h, int type, char* typeString)

void printHeader(HeaderSmv* h);

void letgoHeader(HeaderSmv* h);

typedef struct headerController
{
  HeaderSmv ** headers;
  int CURRENT_SIZE;
  
}HeaderController;

HeaderSmv** initHeadersStruct(int size);

HeaderController* createController(int size);

void letGoHeadersStruct(HeaderSmv** hs, int size);

void letGoHeaderControl(HeaderController* Hcontrol);

void preProcessSmv(FILE* smvP, HeaderSmv** ds);

void postProcessSmv(FILE* smvP, int* ds);

// salva o header do módulo lido anteriormente
void initPreProcessHeader(int type, char* moduleName, HeaderController* Hcontrol);

// salva a linha do baseada no  header do módulo lido anteriormente
void saveLineOnBuffer(int type,int part, char* line, HeaderController* Hcontrol);

// mudar nome dos evals
int computePhase1(int stage, char* buffer, char* varString, HeaderSmv** ds);

int computePhase2(int stage, char* buffer, char* assignString,HeaderSmv** ds, int readAutomata, char* transString);

#endif
