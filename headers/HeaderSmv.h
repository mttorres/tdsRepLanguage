#ifndef HEADER_H

#define HEADER_H

enum headerPart {CREATE_MODULE = -1, VAR = 0, ASSIGN = 1, TRANS = 2};

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

void letgoHeader(HeaderSmv* h);


/*Controlador de Header(auxiliar)*/
HeaderSmv** initHeadersStruct(int size);

void letGoHeadersStruct(HeaderSmv** hs, int size);


typedef struct headerController
{
  HeaderSmv ** headers;
  int CURRENT_SIZE; // usar current size com -1 (se vc está processando o indice atual(ultimo) é porque ele já alocou para o seguinte!)
                    // ex:  sétimo membro(6) (CURRENT_SIZE = 7) (ou seja para referenciar o último é só pegar CURRENT-1)
  
}HeaderController;

HeaderController* createController(int size);

void letGoHeaderControl(HeaderController* Hcontrol);

/*Controlador de Header(auxiliar)*/
void printHeaderBuffer(HeaderSmv* h, int type, char* typeString);

void printHeader(HeaderSmv* h);

#endif
