#ifndef PREP_H

#define PREP_H

#include "HeaderSmv.h"
#include "STable.h"




/* 
	salva o headerSMV do módulo lido anteriormente. 
	se for um automato o ponteiro de transição não é vázio.
	efeito colateral: * ao criar ao salvar o header incrementa o tamanho de headers do HeaderController
					  *	cria um header (necessita de free depois) 
*/

void initPreProcessHeader(int type, char* moduleName, HeaderController* Hcontrol); 




/*
	escolhe um buffer do header atual para salvar a linha (alocando sempre uma string para essa linha, que depois deve ser liberada).
	efeitos colaterais:  * ao ter readVarPortsModule como true, ele salva as variáveis do portsModule em uma tabela de portas
						 * ao ter controlRename como true, ele remove todas as ocorrências de determinados caracteres (no caso [])

*/
void selectBuffer(int part, char* line, HeaderSmv* header, int controlRename, int readVarsPortsModule, STable** writeSmvTypeTable);



/*
	prepara para chamar a função de escolha de buffer, t
	ratando a posição (pos) que representa o tamanho do vetor de HEADERS do controller (LEN-1)
						 
*/
void saveLineOnBuffer(int pos,int part, char* line, HeaderController* Hcontrol, int controlRename, int readVarsPortsModule, STable** writeSmvTypeTable);



/* fases: criação, var, assign(pode não existir), trans(pode não existir) (as partes de interesse)
 	as partes de interesse servem como delimitadores,  quebras de linha servem como delimitadores dos módulos
 	stages são os módulos 0(main), automato(2), ports(3)
*/
void processPhase(int stage, int part, HeaderController* Hcontrol, char * line, int controlRename, int readVarsPortsModule, STable** writeSmvTypeTable);


void preProcessSmv(FILE* smvP, HeaderController* Hcontrol, STable** writeSmvTypeTable); 


#endif

